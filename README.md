# Zero-Trust Vehicle Telemetry Gateway

Plataforma de telemetria veicular simulada, construída para demonstrar princípios de **Zero-Trust Security**, **IoT/Embarcados**, e práticas de **DevSecOps** sobre AWS Free Tier, Kubernetes e Terraform.

![Arquitetura](docs/architecture.png)

## Visão geral

Cada veículo simulado ("ECU") se conecta à nuvem apenas com identidade própria (certificado X.509), nunca em texto plano e nunca com confiança implícita — cada mensagem é autenticada, validada e roteada com o menor privilégio possível antes de chegar a qualquer serviço de armazenamento ou visualização.

A arquitetura foi desenhada em torno de três domínios:

1. **Ingestão de dispositivo (IoT / Zero-Trust)**
2. **Processamento e persistência (serverless)**
3. **Acesso externo (Kubernetes Gateway + painel público)**

Todo o ciclo de vida da infraestrutura é auditado (CloudTrail) e implantado via pipeline de CI/CD com verificação de Terraform antes de qualquer aplicação de mudança.

---

## Componentes

### 1. CI/CD — `CODE`
- Código escrito em VS Code, versionado no GitHub.
- Pipeline de CI valida:
  - build/testes da aplicação;
  - `terraform plan`/checagem de conformidade da infraestrutura antes de qualquer `apply`;
  - autenticação com a AWS via **OIDC** (o GitHub Actions assume uma IAM Role temporária — sem chave de acesso de longa duração armazenada em secret, aplicando Zero-Trust também ao próprio pipeline).
- Após aprovação, a imagem é construída (`Build`) e implantada no cluster Kubernetes.
- Qualquer alteração manual de infraestrutura fora do pipeline (ex: alguém mexendo direto no console AWS) é auditada via **CloudTrail**, que monitora especificamente as chamadas de API relacionadas ao Terraform/infraestrutura.

### 2. Ingestão de dispositivo — `IoT device → IoT Core`
- Cada dispositivo (ECU simulada) possui um **certificado X.509 próprio**, emitido por uma CA dedicada ao projeto.
- A comunicação com o **AWS IoT Core** é feita via **mTLS**: o dispositivo apresenta seu certificado, o IoT Core valida a cadeia, e uma **IoT Policy** restringe o dispositivo a publicar apenas no seu próprio tópico MQTT (`vehicle/{id}/telemetry`) — princípio de menor privilégio aplicado por dispositivo.
- Uma **IoT Rule** roteia as mensagens validadas para o **SQS**, que atua como buffer/fila de desacoplamento entre a ingestão e o processamento — absorve picos de tráfego e permite retry sem perda de mensagem.

### 3. Processamento — `Lambdas`
Três funções Lambda, cada uma com responsabilidade única:

| Função | Responsabilidade |
|---|---|
| `store-car-svc` | Consome o SQS, valida schema/anomalias, grava o estado atual no DynamoDB e o payload bruto no S3 |
| `watch-telemetry-svc` | Expõe leitura de telemetria para consumidores autorizados |
| `public-panel-svc` | Serve os dados agregados/públicos para o painel externo |

**DynamoDB vs. S3 — diferença de papel:**
- **DynamoDB**: guarda o **estado atual** de cada veículo (1 item por `vehicle_id`, sobrescrito a cada nova mensagem). É o que o painel público consulta — leitura rápida e barata por chave.
- **S3**: guarda o **histórico bruto**, um objeto por evento, nunca sobrescrito. Funciona como data lake para análise posterior, auditoria e treinamento de modelos de detecção de anomalia.
- A gravação no S3 ocorre somente **após validação/aprovação** da mensagem (`if req is approved`), e pode ser implementada de forma desacoplada via **DynamoDB Streams**, disparando um Lambda de arquivamento automaticamente após cada escrita no DynamoDB.

### 4. Kubernetes Cluster
Hospeda a camada de roteamento e observabilidade — **não hospeda os Lambdas**, que continuam sendo recursos totalmente gerenciados pela AWS, fora do cluster.

- **Gateway** (nginx/envoy): recebe requisições externas e as roteia para o serviço correto.
- Cada Lambda é referenciado dentro do cluster por um **Service do tipo `ExternalName`**, que mapeia um nome de serviço interno (ex: `public-panel-svc`) para o hostname real da **Lambda Function URL** via DNS (CoreDNS resolve como CNAME). O Gateway então realiza a chamada HTTPS real diretamente para a AWS.
- **Config/Secrets com SSM**: parâmetros e variáveis sensíveis usadas pelos serviços do cluster são centralizados no **AWS Systems Manager Parameter Store**.
- **Prometheus + Grafana**: observabilidade interna do cluster (métricas de Pods, do Gateway, latência).

### 5. Acesso externo — `Clients`
- Clientes autorizados acessam via **CloudFront**, que serve como CDN/edge público e único ponto de entrada HTTPS.
- Resolução de nome via domínio próprio (CNAME apontando para o hostname do CloudFront) ou diretamente pelo domínio nativo `*.cloudfront.net`.

### 6. Segurança transversal
- **IAM**: controla o que cada usuário/role pode acessar (menor privilégio).
- **SSM Parameter Store**: armazena configuração e segredos sem custo adicional.
- **ACM**: certificado TLS público do domínio exposto pelo CloudFront.
- **CloudWatch**: logs e métricas de todas as etapas do pipeline (Lambdas, IoT Core, cluster).
- **CloudTrail**: auditoria de toda alteração de infraestrutura feita via Terraform ou manualmente no console.

---

## Princípios de Zero-Trust aplicados

| Princípio | Onde é aplicado |
|---|---|
| Identidade forte por entidade | Certificado X.509 único por dispositivo IoT |
| Nunca confiar, sempre verificar | Validação de schema/anomalia no Lambda antes de persistir qualquer dado |
| Menor privilégio | IoT Policy por thing; IAM Roles específicas por função Lambda |
| Sem credencial de longa duração | Autenticação da CI via OIDC ao invés de chave de acesso estática |
| Auditoria contínua | CloudTrail monitorando alterações de infraestrutura |

---

## Stack utilizada

- **Linguagem embarcada**: C++ (simulação de ECU)
- **IaC**: Terraform
- **CI/CD**: GitHub Actions (OIDC → AWS)
- **Orquestração**: Kubernetes (kind/k3d local)
- **Cloud**: AWS (IoT Core, SQS, Lambda, DynamoDB, S3, CloudFront, IAM, SSM, ACM, CloudWatch, CloudTrail)
- **Observabilidade local**: Prometheus + Grafana

## Próximos passos

- [ ] Diagrama detalhado do pipeline de CI/Terraform
- [ ] Política de rotação de certificados (mTLS) dos dispositivos
- [ ] Regras de detecção de anomalia (IDS leve) no `store-car-svc`
- [ ] Admission controller (OPA/Gatekeeper) no cluster

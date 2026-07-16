# Zero-Trust Vehicle Telemetry Gateway

A simulated vehicle telemetry platform built to demonstrate **Zero-Trust security** principles, **IoT/embedded systems**, and **DevSecOps** practices on top of AWS Free Tier, Kubernetes, and Terraform.

<img width="1054" height="826" alt="image" src="https://github.com/user-attachments/assets/d953b9d8-7bbe-4200-b31d-c251c4f30880" />

## Overview

Each simulated vehicle ("ECU") connects to the cloud using its own identity (X.509 certificate) — never in plaintext, and never with implicit trust. Every message is authenticated, validated, and routed with the least privilege possible before reaching any storage or visualization service.

The architecture is organized around three domains:

1. **Device ingestion (IoT / Zero-Trust)**
2. **Processing and persistence (serverless)**
3. **External access (Kubernetes Gateway + public dashboard)**

The entire infrastructure lifecycle is audited (CloudTrail) and deployed through a CI/CD pipeline that verifies Terraform changes before any apply.

---

## Components

### 1. CI/CD — `CODE`
- Code is written in VS Code and versioned on GitHub.
- The CI pipeline validates:
  - application build/tests;
  - `terraform plan` / compliance checks on the infrastructure before any `apply`;
  - authentication to AWS via **OIDC** (GitHub Actions assumes a temporary IAM Role — no long-lived access key stored as a secret, applying Zero-Trust to the pipeline itself as well).
- Once approved, the image is built (`Build`) and deployed to the Kubernetes cluster.
- Any manual infrastructure change made outside the pipeline (e.g., someone editing resources directly in the AWS console) is audited via **CloudTrail**, which specifically monitors API calls related to Terraform/infrastructure.

### 2. Device ingestion — `IoT device → IoT Core`
- Each device (simulated ECU) has its **own X.509 certificate**, issued by a CA dedicated to the project.
- Communication with **AWS IoT Core** uses **mTLS**: the device presents its certificate, IoT Core validates the chain, and an **IoT Policy** restricts the device to publishing only to its own MQTT topic (`vehicle/{id}/telemetry`) — a least-privilege principle applied per device.
- An **IoT Rule** routes validated messages to **SQS**, which acts as a buffer/decoupling queue between ingestion and processing — absorbing traffic spikes and enabling retries without message loss.

### 3. Processing — `Lambdas`
Three Lambda functions, each with a single responsibility:

| Function | Responsibility |
|---|---|
| `store-car-svc` | Consumes SQS, validates schema/anomalies, writes the current state to DynamoDB and the raw payload to S3 |
| `watch-telemetry-svc` | Exposes telemetry reads to authorized consumers |
| `public-panel-svc` | Serves aggregated/public data to the external dashboard |

**DynamoDB vs. S3 — role difference:**
- **DynamoDB**: stores the **current state** of each vehicle (one item per `vehicle_id`, overwritten on every new message). This is what the public dashboard queries — fast, cheap key-based reads.
- **S3**: stores the **raw history**, one object per event, never overwritten. Acts as a data lake for later analysis, auditing, and training anomaly-detection models.
- Writes to S3 happen only **after the message is validated/approved** (`if req is approved`), and can be implemented in a decoupled way via **DynamoDB Streams**, triggering an archival Lambda automatically after each DynamoDB write.

### 4. Kubernetes Cluster
Hosts the routing and observability layer — it does **not** host the Lambdas, which remain fully AWS-managed resources living outside the cluster.

- **Gateway** (nginx/envoy): receives external requests and routes them to the correct service.
- Each Lambda is referenced inside the cluster through a **`Service` of type `ExternalName`**, which maps an internal service name (e.g., `public-panel-svc`) to the real hostname of the **Lambda Function URL** via DNS (CoreDNS resolves it as a CNAME). The Gateway then performs the actual HTTPS call directly to AWS.
- **Config/Secrets via SSM**: parameters and sensitive variables used by the cluster's services are centralized in **AWS Systems Manager Parameter Store**.
- **Prometheus + Grafana**: internal cluster observability (Pod metrics, Gateway latency, etc.).

### 5. External access — `Clients`
- Authorized clients access the system via **CloudFront**, which serves as the CDN/edge layer and single public HTTPS entry point.
- Name resolution is done via a custom domain (CNAME pointing to the CloudFront hostname) or directly through the native `*.cloudfront.net` domain.

### 6. Cross-cutting security
- **IAM**: controls what each user/role can access (least privilege).
- **SSM Parameter Store**: stores configuration and secrets at no extra cost.
- **ACM**: public TLS certificate for the domain exposed by CloudFront.
- **CloudWatch**: logs and metrics for every stage of the pipeline (Lambdas, IoT Core, cluster).
- **CloudTrail**: audits every infrastructure change made via Terraform or manually through the console.

---

## Zero-Trust principles applied

| Principle | Where it's applied |
|---|---|
| Strong identity per entity | Unique X.509 certificate per IoT device |
| Never trust, always verify | Schema/anomaly validation in Lambda before persisting any data |
| Least privilege | Per-thing IoT Policy; per-function IAM Roles |
| No long-lived credentials | CI authentication via OIDC instead of a static access key |
| Continuous auditing | CloudTrail monitoring infrastructure changes |

---

## Stack

- **Embedded language**: C++ (ECU simulation)
- **IaC**: Terraform
- **CI/CD**: GitHub Actions (OIDC → AWS)
- **Orchestration**: Kubernetes (local kind/k3d)
- **Cloud**: AWS (IoT Core, SQS, Lambda, DynamoDB, S3, CloudFront, IAM, SSM, ACM, CloudWatch, CloudTrail)
- **Local observability**: Prometheus + Grafana

## Next steps

- [ ] Detailed CI/Terraform pipeline diagram
- [ ] Certificate rotation policy (mTLS) for devices
- [ ] Lightweight anomaly detection (IDS) rules in `store-car-svc`
- [ ] Admission controller (OPA/Gatekeeper) in the cluster

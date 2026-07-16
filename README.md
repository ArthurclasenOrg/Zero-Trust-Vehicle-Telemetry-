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
- **Orchestration**: Kubernetes (local minikube)
- **Cloud**: AWS (IoT Core, SQS, Lambda, DynamoDB, S3, CloudFront, IAM, SSM, ACM, CloudWatch, CloudTrail)
- **Local observability**: Prometheus + Grafana

## CI/CD pipeline

![CI/CD pipeline](docs/ci-pipeline.png)

The pipeline runs on every commit/push and is split into four jobs, with the later jobs gated behind the security checks of the earlier ones — no image reaches AWS without passing static analysis and container scanning first.

### Job 1 — Build
- Builds the three service images (`public-panel-svc`, `watch-telemetry-svc`, `storage-service`) using **CMake**.
- All three images are rebuilt on every commit, even if only one changed. This favors simplicity and consistency over build speed for a personal project — a future optimization would be to use a path-based build matrix so only the changed image is rebuilt.
- Two downstream jobs (Job 2 and Job 3) branch off Job 1 and run in parallel.

### Job 2 — SAST
- Runs **Semgrep** against the source code for pattern-based static security analysis (SAST), including C/C++-specific rulesets.
- *Optional complement*: for deeper C++-specific static analysis (undefined behavior, memory issues, pointer misuse) not fully covered by Semgrep's generic ruleset, **cppcheck** and/or **clang-tidy** can be added as an extra step here.

### Job 3 — Container scan
Two steps, both powered by **Trivy**:
1. **Vulnerability scan** of the built container image.
2. **SBOM check** — generation/validation of the Software Bill of Materials (CycloneDX/SPDX), so every library shipped inside the image is tracked and auditable for known vulnerabilities.

### Job 4 — Release (depends on Job 2 **and** Job 3 passing)
1. **Push image** to the container registry.
2. **`terraform plan`/`apply`** against AWS, authenticated via OIDC (no static AWS credentials).
3. **Deploy to Lambda** — the updated container image is published as the new Lambda function version.

> Implementation note: since each service runs as a C++ binary inside a Lambda container image, the binary must implement (or wrap) the **Lambda Runtime API** — it is not a regular standalone binary. The `aws-lambda-cpp` library (AWS Labs) provides this runtime loop for C++.

---

## Next steps

- [x] Detailed CI/Terraform pipeline diagram
- [ ] Certificate rotation policy (mTLS) for devices
- [ ] Lightweight anomaly detection (IDS) rules in `store-car-svc`
- [ ] Admission controller (OPA/Gatekeeper) in the cluster
- [ ] Path-based build matrix in Job 1 (only rebuild the image that changed)
- [ ] Manual approval gate (GitHub Environments) before `terraform apply` in Job 4

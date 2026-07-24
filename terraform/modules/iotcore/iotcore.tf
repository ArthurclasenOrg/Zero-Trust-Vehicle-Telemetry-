data "aws_caller_identity" "current" {}

# the thing defined
resource "aws_iot_thing" "vehicle_01" {
  name = "vehicle-01"
}

# defining the thing group (vehicles group)
resource "aws_iot_thing_group" "vehicle_devices" {
    name = "vehicle_devices"
    properties {
        attribute_payload {
            attributes = {
                Vehicle = "Zero-Telemtry-Service-Vehicle"
            }
        }
    }
}

# adding thing to group
resource "aws_iot_thing_group_membership" "vehicle_01_membership" {
  thing_name       = aws_iot_thing.vehicle_01.name
  thing_group_name = aws_iot_thing_group.vehicle_devices.name
}

# IoT Policy for devices
resource "aws_iot_policy" "device_policy" {
  name = "vehicle-telemetry-device-policy"

  policy = jsonencode({
    Version = "2012-10-17"
    Statement = [
        {
            Effect = "Allow"
            Action = ["iot:Connect"]
            Resource = "arn:aws:iot:${var.aws_region}:${data.aws_caller_identity.current.account_id}:client/$${iot:Connection.Thing.ThingName}"
        },
        {
            Effect = "Allow"
            Action = ["iot:Publish"]
            Resource = "arn:aws:iot:${var.aws_region}:${data.aws_caller_identity.current.account_id}:topic/vehicle/$${iot:Connection.Thing.ThingName}/telemetry"
        }
    ]
  })
}

# certificate (without CA)
resource "aws_iot_certificate" "cert" {
  certificate_pem = file("../cert_and_keys/iot_device_crt.pem")
  active = true
}

# connecting policy to the certificate
resource "aws_iot_policy_attachment" "device_policy_attach" {
  policy = aws_iot_policy.device_policy.name
  target = aws_iot_certificate.cert.arn
}

# connecting certificate to the thing
resource "aws_iot_thing_principal_attachment" "thing_cert_attach" {
  thing = aws_iot_thing.vehicle_01.name
  principal = aws_iot_certificate.cert.arn
}

# iot rule to send to the SQS
resource "aws_iot_topic_rule" "telemetry_to_sqs" {
  name = "route_telemetry_to_sqs"
  enabled = true
  sql = "SELECT * FROM 'vehicle/+/telemetry'"
  sql_version = "2016-03-23"

  sqs {
    queue_url = var.queue_url
    role_arn = var.iot_role_arn
    use_base64 = false
  }
}

# logs for iot
resource "aws_iot_logging_options" "iot_logs" {
  role_arn = var.iot_role_arn
  default_log_level = "ERROR"
}




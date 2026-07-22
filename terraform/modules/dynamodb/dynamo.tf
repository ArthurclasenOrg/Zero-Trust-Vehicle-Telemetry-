# DynamoDB tables

# table for watching by public-panel-svc
resource "aws_dynamodb_table" "vehicle-telemetry-state" {
    name = "vehicle-telemetry-state"
    billing_mode = "PROVISIONED" 
    hash_key = "vehicle_id"
    read_capacity = 5
    write_capacity = 5
    # configured read/write capacity for security reasons (to not be charged if it pass through the 25 RCUs/WCUs from free-tier)

    # vehicle id (hash that refers to that vehicle)
    attribute {
      name = "vehicle_id"
      type = "S"
    }

    # light IDS result run on store-car-svc ("ok" or "anomaly")
    attribute {
      name = "status"
      type = "S"
    }

    # time to live to avoid existing data from vehicles that doesn't generate data (meaning this data is not valid for our case)
    ttl {
      attribute_name = "TimeToExist"
      enabled = true
    }

    # to consult every vehicle with anomalies without Scan (dissalowed on IAM)
    global_secondary_index {
      name = "Vehicles-with-anomalies"
      hash_key = "status"
      projection_type = "ALL"
      read_capacity = 5
      write_capacity = 5
    }

    tags = {
        Project = var.project_name
    }
} 

resource "aws_dynamodb_table" "vehicle-telemetry-public" {
    name = "vehicle-telemetry-public"
    billing_mode = "PROVISIONED" 
    hash_key = "public_alias"
    read_capacity = 5
    write_capacity = 5
    # configured read/write capacity for security reasons (to not be charged if it pass through the 25 RCUs/WCUs from free-tier)

    # pulic alias (for pseudonymity)
    attribute {
      name = "public_alias"
      type = "S"
    }

    # "ok" or "alert" to indicate status without exposing ids internal logic
    attribute {
      name = "status"
      type = "S"
    }

    # time to live to avoid existing data from vehicles that doesn't generate data (meaning this data is not valid for our case)
    ttl {
      attribute_name = "TimeToExist"
      enabled = true
    }

    global_secondary_index {
      name = "Vehicles-with-anomalies"
      hash_key = "status"
      projection_type = "ALL"
      read_capacity = 5
      write_capacity = 5
    }

    tags = {
        Project = var.project_name
    }
}
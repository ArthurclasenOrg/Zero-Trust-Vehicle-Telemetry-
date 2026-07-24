

resource "aws_cloudfront_distribution" "vehicle_tel_distribution" {
  enabled = true
  is_ipv6_enabled = true
  
  # origin config, pointing to my k8s gateway
  origin {
    domain_name = trimsuffix(trimprefix(var.public_pannel_svc_function_url, "https://"), "/")
    origin_id = "api-gateway-origin"

    custom_origin_config {
      http_port = 80
      https_port = 443
      origin_protocol_policy = "https-only"
      origin_ssl_protocols = ["TLSv1.2"]
    }
  }

  # cache pattern behavior  
  default_cache_behavior {
    allowed_methods = ["GET", "HEAD"]
    cached_methods = ["GET", "HEAD"]
    target_origin_id =  "api-gateway-origin"
    viewer_protocol_policy = "redirect-to-https"
    cache_policy_id        = "4135ea2d-6df8-44a3-9df3-4b5a84be39ad"
  }

  # ACM certificate to cloudfront (default one)
  viewer_certificate {
    cloudfront_default_certificate = true
  }

  # no Geographic restrictions (can access from anywhere)
  restrictions {
    geo_restriction {
      restriction_type = "none"
    }
  }

  tags = {
    Project = var.project_name
  }
}

#include <pgmspace.h>
 
#define SECRET
#define THINGNAME "signageMonitoring_sys"                         //change this
 
const char WIFI_SSID[] = "JioFi4_1C5391";               //change this
const char WIFI_PASSWORD[] = "am11p7sedp";           //change this
//const char WIFI_SSID[] = "OnePlus95g";               //change this
//const char WIFI_PASSWORD[] = "password";           //change this
const char AWS_IOT_ENDPOINT[] = "a1ujz06l179gqq-ats.iot.ap-south-1.amazonaws.com";       //change this
 
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUKfHx+6ymb68/xXUeTQa07hMTr30wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIyMTIwNjA5MzUw
NloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK/+1gzuDgIakvO/Eff8
3oYhP0aInzqR6gYcut8H+m+M+eetaXWatihMCWqMUuLU4sZUr2Vck+yyEb5VOFc6
RxNihHF1IKjwpgRhnuUZ22G15DVr/4vTLhDaAxYm4jShAm7J0D8i8DFqlhJ0gp/M
O1gnjac+ls3C0xZXp/98JM35tJ1Jw0jv4/xAif66J8fnFDjLu5airdVLZBQ0Ctz+
xWgYgoQOb4KqleCBgS64xMi5VrCSUUuK2/ZaaVFrFctJ7j4rH/IYMNpcSVY4tPaP
nm1puwPZTnPjZt79KgDuw2ByGb/3Q/wBSZf7guYl2TO53xiOFuJQvs9ZnNxhrM+5
K0MCAwEAAaNgMF4wHwYDVR0jBBgwFoAUC/rWd4baif/xPB7yvAft78EV47owHQYD
VR0OBBYEFFhr1GGkNqM+HUci2QIk+OWfYkO+MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQB2Ve7iU0mHqAFlkeuUU+SPg4U4
rvIs48E7ReN0LWbLFNV2BafItWTVXaLg9sZ1/e5PBZvvlroMbjdgsMlas+FSOkp+
vN0Oneuy8nTokmpvTZEdrnS4n8Lte2SuswmSqFeGZJDd4zgpnqEO7n0VMZzBwYDK
KK5Om9N4R9ArF88cshJp1DMqdEkyPMgGbhVhnzOvO6vnf76kyXTiDGehm+eb52cG
pj614I+bZlF0M6p1Yb8Yv9q1aW1ixN9oVMOQqGzzZO1cghPII8YB5/Ybopj78lB5
pj046HxL/rWzDJp9xv6BMBrJFM0nZG3Sr86m6kGdIcShGNdg9btWWlAuK189
-----END CERTIFICATE-----
 
 
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAr/7WDO4OAhqS878R9/zehiE/RoifOpHqBhy63wf6b4z5561p
dZq2KEwJaoxS4tTixlSvZVyT7LIRvlU4VzpHE2KEcXUgqPCmBGGe5RnbYbXkNWv/
i9MuENoDFibiNKECbsnQPyLwMWqWEnSCn8w7WCeNpz6WzcLTFlen/3wkzfm0nUnD
SO/j/ECJ/ronx+cUOMu7lqKt1UtkFDQK3P7FaBiChA5vgqqV4IGBLrjEyLlWsJJR
S4rb9lppUWsVy0nuPisf8hgw2lxJVji09o+ebWm7A9lOc+Nm3v0qAO7DYHIZv/dD
/AFJl/uC5iXZM7nfGI4W4lC+z1mc3GGsz7krQwIDAQABAoIBAEn0SNgIFITXy9Z2
/ML7VDqCsAcgk+BVvkQxQGlF/ftv/vdWjFEp4M+QHAjUzGOa12xF26o5O8OjlAcp
4lHtID7KECNo939mGC0RJGba9a/acfns1uZXqCzWn6NoVXeODOqnDUXqlYn+Yu+A
W/9wH6PMgx67Ez2FV5Q041hdP035pHem0qb3ek+iTRLU7KTUgziirr+PlclojYv7
vPynDWzga1ljEBz5qmrM/mXAH2tj5waN3KIzujSgjE47KPp0sA0e0CdlVkq1j9tF
xNkFQHj6hivaQsjITnScJ65CaQBtUUxVh7byB0QytJC1F0udIYqlfYEjIg1yO4Ky
oXkEDMECgYEA2RNCjAHuFS8SqimwwxUXsLp6rYAbb4shSJETzPfPbxmdAdJrU454
RQ3S00S3fDLSYZfbMmdWjxeaOI0/Sz4ebYwl/qxh4PKl7LMBPUhnwzBEgzSM+6fV
zxXRVKCvu8wtOv1ZOrgVGQ22f1UMyI+0jQozyJPXV8OoV1sYBlB+riECgYEAz43T
2Arx6yTHNCnVf8n+HmLionnvhYY44QuWU/XJeyy/wExdGzp8JkMQ6EkUJxQNz3jF
Xprf9GgvCRfp3zqnPF9MCV7fIiQU0lWntIYiyRmzOu3/nfHeNZ8va0TEQ/LErD/f
OlpDWJgVZKihCUWUZtWg+zRf67FhTJk0arykROMCgYBmWvSH++j9SCLu5ZWZhvqS
tHtgEAXfxKS/CNljTn4bDuK/ly5QSd9gADKmt8Ik5/nXVrlevOdNtX1cqQ1tNy81
zF2mBjZo0UF3qLgu41M58Cp+C2tmxi7E5K69zr7OOGojZI6MZ3xbQeIJjFrovwoN
bwAX1c42beWUuzlK0O3KwQKBgQCxIc5o87ZQwvuLJiTCQaYxUp0uorfrEYjbbyMl
ln8UMzWWrNVNFpW7pcL5Kt0bGL9MkDqJKc317TAvJ4x96bWuDXp8CM6PeZGXjGhq
5Yssa1WN9m4P4sqMosU6W97QSpDUnFIrn7sYEfs4A74jhpFHTgxsNt+h66kpURrt
+P6MlwKBgBQ/RUUIg0k5mb0wXlnBXPDRIWcXkluTwK6uIa1M6yTm0N+EB5cuy8bP
jhy7/8cojph0zJ7+ywGDzQ/5vRx1d7L3ulB/4/Qx+dBLxrGgPw+YHbzjx5o1tuO8
JdRsZphYCCUYA9MoCmQeTmaBXw3W5DjI8/A5dml7hV/Ww1jjxG5C
-----END RSA PRIVATE KEY-----
 
 
)KEY";

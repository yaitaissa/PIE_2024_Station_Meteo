#include <pgmspace.h>
#define SECRET
#define THINGNAME "CartographieLumiere" //change this
const char WIFI_SSID[] = "HUAWEI P30"; //change this
const char WIFI_PASSWORD[] = "tomti7676"; //change this
const char AWS_IOT_ENDPOINT[] = "a13loltpsesfzp-ats.iot.eu-west-3.amazonaws.com"; //change this
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

// Device Certificate
//change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIURlivTEn5E1hwutBDe/DBykKbu2wwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMTEyOTE0MjMw
MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL1g/I16+E2j5jBysChx
1dmT/LxE1UZR4/D4sVxlP6GmBDO7hl9901U+Q2ZFvXXBjumgIxSAmAw20zYoG1UC
shKdMIZqlYSjP4+NWafbQ47IjFtH/2qucV1U+5GAN0IbGtw69GeZ9Nmq4IFAKVeY
YQtpy6zF5aiGmNn6cSkMqnnX1RFJfzYnhzaeHMPPUjpyz5nOJwy1j9JraAfdM8C6
VTm7PzBIxa/DIUnYWhuhETAcH6PyP1fdtVjfD6EIFtoHKEnbAbaPLowLNz7U9nDl
SpqQx615xhQqnYAL2ayk0WUbLNnUgospNauKgycC76CD0Nnw0ir1R24pd4+oQiBo
J5sCAwEAAaNgMF4wHwYDVR0jBBgwFoAUTNIIyaTEitC1YZC+j41rY8UIHuIwHQYD
VR0OBBYEFMkPdZngsGY7R87FKOCCYcnAFxM1MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQDB4X1jfXdc2SO/E2sIc/wR4Lzh
SD4xO16gv2FsbDNtZ6v+7R/N0SY51+zT5hufkKmF7XMpZpX/LMPMLvYFUQpE/KCz
jfyXNRlugzUNTzuIF7rsCeYgA12zkzQJEVHjF8MPMGJshAIP5h1Vi4WlzweKG6Mw
StW8tNzlqX4QABTB2Z4wlylveooZ8glJDII5QTTPadgsmeaNj6bdnPtYc47ksFCg
dKcjn/Sqmbldh40AZdf1IJf4HFW6HpzYYaVuf8jmbULvJRL2tySrzqcHrqgH88CN
0yPP1VoxYeeuH+Xdu448gnIZ3r1OrQIvfcJPK3BIrn3CXL1EJ/x+yTPhcu6k
-----END CERTIFICATE-----
)KEY";

// Device Private Key
//change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAvWD8jXr4TaPmMHKwKHHV2ZP8vETVRlHj8PixXGU/oaYEM7uG
X33TVT5DZkW9dcGO6aAjFICYDDbTNigbVQKyEp0whmqVhKM/j41Zp9tDjsiMW0f/
aq5xXVT7kYA3Qhsa3Dr0Z5n02arggUApV5hhC2nLrMXlqIaY2fpxKQyqedfVEUl/
NieHNp4cw89SOnLPmc4nDLWP0mtoB90zwLpVObs/MEjFr8MhSdhaG6ERMBwfo/I/
V921WN8PoQgW2gcoSdsBto8ujAs3PtT2cOVKmpDHrXnGFCqdgAvZrKTRZRss2dSC
iyk1q4qDJwLvoIPQ2fDSKvVHbil3j6hCIGgnmwIDAQABAoIBAQCReHzHmRw/V5Us
xgx3JOpmVf0gCafiemkj44UqOkQkwcgcjQHtPFQn+MoBN7UPTX2gDHil+RnS2BJq
WWiVZpqvHbv2pMzxtAzREn06KX29omPegnG62wEN9oe//sNzrip13Xt8UZD52v7V
ihm3xb9V6xCfSOzfBXVIi858/9O7/zKc0zxLCMVGnqN8dqaCmeck+tMvh9b1vBb8
onQMci/uZsuK7mXEXAJdPykplMu+SaK7d6sz9/4+uDMqiaGj8U/JNOVKmePv6ldD
ni+Hct9XaMlBw7/9iuJy2BudbzlaiaOL9wrN75/MMOPInlBY/4utoaAYY8hLmwBR
/jaWwXvxAoGBAOPsz1BvPBeVjGvnO9OTy6iXlrwsw7AOra00iKxq8scvbrkLTJye
Q+nhSR3MbhkJwR6YCRMyNeXQk1TAL5KUWkX2SvLNUbyJ5/Cs5FsmnNKqHrpEqEGI
1FwlfmrAk/rDryUJZeYaSBCww7BjpVn9lt4uuqmIAZZaSVcuXzITBnh9AoGBANS0
sd7ON3VeT5E3PrBOjfYlOSfhXQww9MIiZeLYFPhjtBmYyS7Ci4TcLe7WX+KqE3CR
5OMlQ1DGUBf8IQVTtKfanMoCS0EJ6BaBO46PA0kC7K8gcMSFGRcgoQeFiyTfd33/
JFKEailyAuZyCDgaOa4vbkU5vUlc133+ang5HTP3AoGBAIdDKUE10ja41I3Qu4tz
gYB6/cSM2FEqVN2eeKHCEnQs7Qu57exgbDNnNEoP4SLQzCAI4BHZUW0iW90G9PSL
QAuW/6GaQbCfMtt870Q85QKMXRHz3tqNhmFtCtn1kZxW9yEGs8dbv3rQPEH40M9Y
FGmk17f2c+KzkCTB61K/Qa7NAoGAGPY3pYkX0wtFVSphjfk6CbfmlDqLW+Hm5114
DX4EP/4bZaGbgoRMMURRk6TOzd/dtik47jsyQilzvszU5zYijkxGeCJ6yGAsP2/d
GGbKIOhZ9FdfRIDGKAOpQiO255XeBQhLL+/gMYrysi6RrlvNhxaD31XeU9y0YVHZ
FtDFD5MCgYAo2PYxJpTnmac0Qmg4v6VE2EfXEcsTbGoTrtBtiKvbt1oqaO0dWQVv
7F90D09PnnOt+FgslnncWuWLWVdClQcwZS8Rz6NlJE63WX9sOhjWqS9wbWeqlUtH
Mm0d3V95AXYGzr9y39KcjIrnQq25jIySLiu7VGp6vR8baqyvaFreaw==
-----END RSA PRIVATE KEY-----
)KEY";
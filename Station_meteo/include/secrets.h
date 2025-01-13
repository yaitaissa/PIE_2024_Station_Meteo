#include <pgmspace.h>

#define SECRET
#define THINGNAME "Test_humidite_temperature" //change this

const char WIFI_SSID[] = "iPhone de Lucie"; //change this
const char WIFI_PASSWORD[] = "lucieeeg"; //change this
const char AWS_IOT_ENDPOINT[] = "ae87uomkyz73q-ats.iot.eu-north-1.amazonaws.com"; //change this
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
MIIDWjCCAkKgAwIBAgIVAP8J0e+9GYIzi8ws1NnDD3WDKpEwMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMzEyMDYwOTQ4
MzRaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDJyQs+e4tym3vQQTyY
rFUv6ThfFJWQngOrcCwuOQdJB5jBs4Ig69IW/0C+qHvWWTQZmn1EkuwvEW/2tvmX
HxKv6slQdtKLIrtSwlZe9EUjUI+WbSIxG05yuNwONx2JyhOh5WTksu0sxsPh504I
sgCaIYco5Vm+v2Po++K/5TMTCp70u+KEer4JjU+I6UuawXfQJYX66oyv+Ld0bV5Q
Dglfvn5XOg2Oh64l3cdoEZPV0n0+umn8Car5hA/Rq7bz7Pc+k9UbL06W+4GZkmVg
eeUgrKu30qV424gIUqCPqbJxlx1UXjYl4I7I22RQZlAvoMkXhP/lwHx3i3TKk17z
IrNZAgMBAAGjYDBeMB8GA1UdIwQYMBaAFF+JM4x4hsjKJHSPcWr+sKVDuEv9MB0G
A1UdDgQWBBT43w1lhRwfZS69jnMxgby8ltG/mTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAm6onNZA6mzEM1RwcIA4KAK5D
eqhisG0t5P9KhKlJckSGoBS/QrXO135+RYUbWLHBXJRueXyMob8RgD/MYy5lX2Qc
xTRRxTmXwsjNzvTcytz2Ok4fk3oSJJ9UCICZTtgJNmz9zjnQIiSEgNk2QX8AiPB7
2PDfEpeuz2jLqDmi2Olc20w5zkwuKaSlq62A8t0goWlOzljeV3sXc0SarXj54T3/
vzDXM3uc9R5FtGPFNjM5o7GnoJn2flLqKN4jfUt8OEDTd3GXPVGnxptNFL4STybC
HKtEvADrdN8+Ceh+qCNSm9j2E4HY95Hs39SSZLDlBdBCCXyyevLdcBEtbMnPWg==
-----END CERTIFICATE-----
)KEY";
// Device Private Key
//change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAyckLPnuLcpt70EE8mKxVL+k4XxSVkJ4Dq3AsLjkHSQeYwbOC
IOvSFv9Avqh71lk0GZp9RJLsLxFv9rb5lx8Sr+rJUHbSiyK7UsJWXvRFI1CPlm0i
MRtOcrjcDjcdicoToeVk5LLtLMbD4edOCLIAmiGHKOVZvr9j6Pviv+UzEwqe9Lvi
hHq+CY1PiOlLmsF30CWF+uqMr/i3dG1eUA4JX75+VzoNjoeuJd3HaBGT1dJ9Prpp
/Amq+YQP0au28+z3PpPVGy9OlvuBmZJlYHnlIKyrt9KleNuICFKgj6mycZcdVF42
JeCOyNtkUGZQL6DJF4T/5cB8d4t0ypNe8yKzWQIDAQABAoIBAFu4C2laRLdSX0P4
8y8+XfZKHvpw4875VBewBBT6jQov7spU6thF+JfFbEt9T9TNdTYSF9B8q+h1F5xf
7TE4yYi2wszVRbxCjD0A4ruEcoyryKiKcJk7nTveNQAtIzsHZEcZevPNNKs+qbjc
J4ciSc3FYadAILvoSOSU4atx5fZH9foN4h/lfqA5CfH5jOrTxhmoeP5Zj0nm0QtB
xzriiuQ5swaQZTolwXXy9ahoKz+oZlqMclkwzqJGHyoBudqopyJAE5L/SyxxadnF
+2BofVM3xcc/nrFs6icyO/KtFU/DFq5XHQyGQczW3jnB3tOkLIkUva/0t/4EPJ7J
gl2RddECgYEA5VDnc/no+bpNO5kYdW38tLkCy2EXJsQJRAJEIK5bFc3yXoh4HAIg
9677Auma1Iw0crRmlxrvZbWBSJYfndBkaeQLkkzNB+vzz/gwZ+0gg3B19ZpbF5qG
ZzrG1FPW9Kfds5q8rCBgHMD1yXcHYm45jGOl8/9FWrwfOQt5u615p7UCgYEA4UQH
EJZhaewzicHgtdMcxYTJjeXICk7ZS/LEu3tWnzBxJfv7pw2Mc45IdgnL1k/RBPRv
vu18I1QBh0SdWNu8lKaNDsvH9vi/Q1Pisp8IT2lxcPgqpiMNPqyGhRzD2TQWBAXO
2cz17ZvIDOIVhjaYuicCINZdOdKfv2LAFKqUG5UCgYEApfkarJxy/VzUTZCPXV8Q
CpXl6eIJ6ar1sd2aBbVsg6meBMHtfQglaF+jmgYOZExeIa9kmXWLVCg8vJa7p3OP
Wf0OSwN/Wn5y51GRZ4b/g2LAXdUsnTbglJK8Gi3cZROiqT4T1NlR9z3KJ9kY5Ss2
R+BP2vwl7HhAeP9PSoFSjgkCgYATSw1D2W1cchNz7/v79qnXLOlKUuzQHTK1M4Sv
/f9MQQzf0PYbt6hY2U3RdkNUBqADSaAsfRCNwgsjd7f2PVOoc59PLhrhRyqybIPV
ApPmoURLBaY5dnUS8hnPWshXWQOtsoWl180UXaxDljV7uj9YajDfeRyg8C4oaR/E
ZVusBQKBgDB4HLGIRfX1HVBJaftyQf+flPaTjGEc9UxRdpNojQw0Le5LKPHmurE0
pgf/v0E4zsP+dDWX3pjUyFgpKykjmRKo7NXnVs3a4lxAsieLBeJDa9EB5KB4aOR0
U/gdrKi2+bcNZ/BCb8VhF+/TCKd2sPBQkL2PtkO/toItc7PCPkA0
-----END RSA PRIVATE KEY-----
)KEY";

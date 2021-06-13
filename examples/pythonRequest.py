import requests
key = "ans"

i = 0
while i < 1000:
    r = requests.get("http://localhost:8000/getLock/3/Hello/20")
    if r.text != "false":
           key = r.text

    r = requests.get("http://localhost:8000/releaseLock/Hello/"+key)
    i = i + 1

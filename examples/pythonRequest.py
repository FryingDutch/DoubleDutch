import requests
key = "ans"

i = 0;
while i < 1000:
    r = requests.get("http://localhost:8000/getLock/Hello")
    while r.text == "false" or r.status_code != 200:
           r = requests.get("http://localhost:8000/getLock/Hello")
           print(r.text)
    else:
           key = r.text
           print(key)

    r = requests.get("http://localhost:8000/releaseLock/Hello/"+key)
    i = i + 1

import requests

data = requests.get("http://localhost:8000/getLock?auth=randomapikey&lockname=hello&timeout=3&lifetime=10").json()
key = data["DoubleDutch/v0.1"]       
#print(key)

data = requests.get("http://localhost:8000/releaseLock?lockname=hello&key="+key).json()

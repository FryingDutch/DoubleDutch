import requests

data = requests.get("http://localhost:8000/getlock?auth=randomapikey&lockname=hello&timeout=3&lifetime=10").json()
token = data["DoubleDutch/v0.1"][0]       
#print(key)

data = requests.delete("http://localhost:8000/releaselock?lockname=hello&token="+token).json()

import requests
port = 8000
auth = "randomapikey"
name = "hello"
timeout = 3
lifetime = 10

i = 0
while i < 10000:
        data = requests.get("http://localhost:"+str(port)+"/getlock?auth="+auth+"&lockname="+name+"&timeout="+str(timeout)+"&lifetime="+str(lifetime)).json()
        key = data["sessiontoken"]       
        print(key)
        
        data = requests.delete("http://localhost:8000/releaselock?lockname=hello&token="+key).json()
        i = i +1

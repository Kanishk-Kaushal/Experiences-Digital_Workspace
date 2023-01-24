yolov7 object tracker
Features:-
  1. --dwell time : to calculate average dwell time of objects
  2. --classes : to predict and monitor a specific class (0 for persons)
  3. --sendData : to send the desired data to a server in real time and save data in json format when internet is down.

Steps:
  1. Download Weights: "wget -P weights https://github.com/WongKinYiu/yolov7/releases/download/v0.1/yolov7.pt"
/*NOTE*/ Additional required files will be automatically downloaded
  2. Make Track_Counter.py executable 
      chmod a+x Track_Counter.py
  3. python3 Track_Counter.py --source 0  --dwell-time --nosave --classes 0  --save-txt --count --sendData

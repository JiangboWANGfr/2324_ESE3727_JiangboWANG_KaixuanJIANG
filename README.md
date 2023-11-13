# TP Bus et Réseaux de Jiangbo WANG et KaiXUAN JIANG

## Introduction
在这个TP中，我们将使用STM32和Raspberry Pi来实现一个温度和压力的监测系统。STM32将会采集温度和压力的数据，然后通过UART协议将数据发送给Raspberry Pi。Raspberry Pi将会通过Flask框架来实现一个web服务器，最后，我们将会通过web服务器来实现对数据的可视化。
同时，使用REST API来实现对温度和压力数据的增删改查
最后，为了学习使用CAN总线，我们会从Raspberry Pi中的web服务器发送一个scale的数据给STM32，STM32会将这个数据和温度数据转换成角度数据，然后通过CAN总线发送给步进电机，步进电机会根据这个角度来转动，从而实现使用温度数据来控制步进电机的转动。下面是系统的架构图：
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/other/TPobjuctive.png)
## Communication Protocol
Raspberry与STM32之间的通信协议如下：
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/other/protocol.png)

## Architecture
### STM32

### Raspberry Pi


## Web server with Flask 
### web index 
当你在浏览器中输入http://172.20.10.13:5000/时，你会看到如下的界面：
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/index.png)
### show data 
等你点击show data按钮连接后，你会看到下图中的界面，这个界面会实时的显示温度和压力的数据，这个网页会每隔一秒自动向stm32发送"GET_T"和"GET_P"的请求，
然后获取最新的温度和压力数据，然后显示在网页上。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/showGraph.png)
## REST API TEST 
我们使用CHROME的插件RESTED来测试REST API.
### 获取最新的温度数据
当在RESTED中输入http://172.20.10.13:5000/temp/，然后选择POST，然后点击send,flask会向stm32发送"GET_T"的请求，然后获取最新的温度数据，并返回json格式的数据。如下图所示，并且会将这个数据存储在一个python列表中。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getNewTemp.png)

### 获取所有的温度数据
当在RESTED中输入http://172.20.10.13:5000/temp/，然后选择GET，然后点击send,flask会返回存储在python列表中的所有的温度数据，并返回json格式的数据。如下图所示。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getAllTemp.png)
### get temp index x 
当在RESTED中输入http://172.20.10.13:5000/temp/<index>,然后选择GET，然后点击send,flask会返回存储在python列表中的第index个温度数据，并返回json格式的数据。如下图所示。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getTempIndex.png)
### get pressure with post
当在RESTED中输入http://172.20.10.13:5000/pres/，然后选择POST，然后点击send,flask会向stm32发送"GET_P"的请求，然后获取最新的压力数据，并返回json格式的数据。如下图所示，并且会将这个数据存储在一个python列表中。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getNewPres.png)
### get pressure with get
当在RESTED中输入http://172.20.10.13:5000/pres/，然后选择GET，然后点击send,flask会返回存储在python列表中的所有的压力数据，并返回json格式的数据。如下图所示。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getAllPres.png)
### get pressure with get x
当在RESTED中输入http://172.20.10.13:5000/pres/<index>,然后选择GET，然后点击send,flask会返回存储在python列表中的第index个压力数据，并返回json格式的数据。如下图所示。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getPresIndex.png)
### set scale
当在RESTED中输入http://172.20.10.13:5000/scale/，然后选择POST，然后点击send,flask会向stm32发送"SET_T"的请求，设置最新的scale数据，并返回json格式的数据。如下图所示，并且会将这个数据存储在一个python列表中。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/setScale.png)
### get scale
当在RESTED中输入http://172.20.10.13:5000/scale/，然后选择GET，然后点击send,flask会返回存储的最新的scale数据，并返回json格式的数据。如下图所示。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getScale.png)
### get angle 
当在RESTED中输入http://172.20.10.13:5000/angle/，然后选择GET，然后点击send,flask会向stm32发送"GET_A"的请求，然后获取最新的角度数据，并返回json格式的数据。如下图所示。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/getAngle.png)
### delete  temp inex 
当在RESTED中输入http://172.20.10.13:5000/temp/<index>，然后选择DELETE，然后点击send,flask会删除存储在python列表中的第index个温度数据，并返回删除的index。如下图所示。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/deleteTempIndex.png)
### delete  pres index 
当在RESTED中输入http://172.20.10.13:5000/pres/<index>，然后选择DELETE，然后点击send,flask会删除存储在python列表中的第index个压力数据，并返回删除的index。如下图所示。
![image](https://github.com/JiangboWANGfr/2324_ESE3727_JiangboWANG_KaixuanJIANG/blob/main/pictureforReadme/webflask/deletePresIndex.png)

## Result de Protocol 

## Conclusion


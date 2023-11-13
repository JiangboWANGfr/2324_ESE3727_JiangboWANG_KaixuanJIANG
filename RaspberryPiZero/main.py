"""
    File name: main.py
    Created on: 7 June 2019
    Last Modified: 13 November 2023
    Author: Jiangbo WANG

    Description:
        This module is used to create a web server using Flask.
"""

from flask import Flask, jsonify, request, abort, render_template
import serial
import json
import random
import time
from getDataFromSTM32 import get_temperature, get_pressure,get_angle,set_scale

app = Flask(__name__)

# set the global variables
temperatures = []
pressures = []
scale_global = 0
'''
the route /getdataforgraph is used to get the temperature and pressure to set the graph
'''
@app.route('/getdataforgraph', methods=['GET'])
def create_temperature():
    temperature = get_temperature()
    pressure = get_pressure()
    
    # the temperature is "T=+12.50_C"
    if temperature.startswith("T=") and pressure.startswith("P="):
        # get 12.50
        temperature = temperature.split('+')[1].split('_')[0]
        pressure = pressure.split('=')[1].split('P')[0]
        return jsonify({'temperature': temperature,'pressure':pressure}), 201
    else:
        temperature = 0
        pressure = 0
        abort(400)

'''
The route /temp/ is used to get the temperature using POST method
'''
@app.route('/temp', methods=['POST'])
def create_temp():
    temperature = get_temperature()
    # the temperature is "T=+12.50_C"
    if temperature.startswith("T="):
        # get 12.50
        temperature = temperature.split('+')[1].split('_')[0]
        temperatures.append(temperature)
        return jsonify({'temperature': temperature}), 201
    else:
        temperature = 0
        temperatures.append(temperature)
        abort(400)


@app.route('/temp/', methods=['GET'])
def get_all_temps():
    return jsonify({'temperatures': temperatures})

@app.route('/temp/<int:index>', methods=['GET'])
def get_temp(index):
    try:
        return jsonify({'temperature': temperatures[index]})
    except IndexError:
        abort(404)

'''
The route /pres/ is used to get the pressure using POST method
'''
@app.route('/pres/', methods=['POST'])
def create_pres():
    time.sleep(0.5)
    pressure = get_pressure()
    # the pressure is "P=102300Pa"
    if pressure.startswith("P="):
        pressure = pressure.split('=')[1].split('P')[0]
        pressures.append(pressure)
        return jsonify({'pressure': pressure}), 201
    else:
        pressure = 0
        pressures.append(pressure)
        abort(400)

'''
The route /pres/ is used to get all the pressure using GET method
'''
@app.route('/pres/', methods=['GET'])
def get_all_press():
    return jsonify({'pressures': pressures})

@app.route('/pres/<int:index>', methods=['GET'])
def get_press(index):
    try:
        return jsonify({'pressure': pressures[index]})
    except IndexError:
        abort(404)
        
'''
The route /scale/ is used to update the scale using POST method
'''
@app.route('/scale/<int:new_scale>', methods=['POST'])
def update_scale(new_scale):
    global scale_global
    scale_global = new_scale
    returnValue = set_scale(new_scale)
    if returnValue ==True:
        return jsonify({'scale': new_scale}), 200
    else:
        abort(400)
'''
The route /scale/ is used to get the scale using GET method
'''
@app.route('/scale/', methods=['GET'])
def get_scale():
    return jsonify({'scale': scale_global})


'''
The route /angle/ is used to get the angle using GET method
'''
@app.route('/angle/', methods=['GET'])
def get_angleweb():
    angle = get_angle()
    return jsonify({'angle': angle})
    if angle.startswith("A="):
        angle = angle.split('=')[1]
    else:
        abort(404)

'''
the route /temp/<int:index> is used to delete the temperature index using DELETE method
'''
@app.route('/temp/<int:index>', methods=['DELETE'])
def delete_temp(index):
    try:
        del temperatures[index]
        return jsonify({'delete temp index': index})
    except IndexError:
        abort(404)

'''
the route /pres/<int:index> is used to delete the pressure index using DELETE method
'''
@app.route('/pres/<int:index>', methods=['DELETE'])
def delete_pres(index):
    try:
        del pressures[index]
        return jsonify({'delete pressure index': index})
    except IndexError:
        abort(404)


'''
The route /showdata/ is used to show the data graph in the web
'''
@app.route('/showdata/')
def data():
    return render_template('showdata.html')

'''
This is the index page
'''
@app.route('/')
def index():
    return render_template("index.html")

'''
This is the 404 page
'''
@app.errorhandler(404)
def page_not_found(error):
    # 返回自定义的 404 错误页面
    return render_template('404.html'), 404

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)

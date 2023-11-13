"""
    File name: main.py
    Created on: 7 June 2019
    Last Modified: 9 November 2023
    Author: Jiangbo WANG

    Description:
        This module provides an interface for executing shell commands and parsing their output.
"""
from flask import Flask, jsonify, render_template, abort, request
import random
app = Flask(__name__)

# data 
welcome = ["Hello", "Bonjour", "Hola", "你好", "こんにちは"]


@app.route('/')
def index():
    return render_template("index.html")

# api route
@app.route('/welcome/<int:index>')
def api_welcome_index(index):
    try:
        # return welcome[index] with json format
        return jsonify({"index": index, "val": welcome[index]})
    except IndexError:
        # if index out of range, return 404
        abort(404)

# return 404 page if not found
@app.errorhandler(404)
def page_not_found(error):
    # 返回自定义的 404 错误页面
    return render_template('404.html'), 404

def get_temperature():
    # 这里应该是读取温度传感器数据的代码
    return random.uniform(20, 30)

def get_pressure():
    # 这里应该是读取压力传感器数据的代码
    return random.uniform(1, 5)

@app.route('/temp')
def data():
    return render_template('temp.html')


@app.route('/temperature')
def temperature():
    return jsonify(temperature=get_temperature())

@app.route('/pressure')
def pressure():
    return jsonify(pressure=get_pressure())

# run app
if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
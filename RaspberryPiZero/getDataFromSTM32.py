"""
    File name: getDataFromSTM32.py
    Created on: 7 June 2019
    Last Modified: 9 November 2023
    Author: Jiangbo WANG

    Description:
        This module is used to get data from STM32 via serial port.
"""
import serial
import time

# 初始化串行通信
ser = serial.Serial('/dev/serial0', 115200, timeout=1)

def send_command(command):
    """
    Send a command to STM32 via serial port.
    """
    try:
        ser.write(str(command).encode('utf-8'))
        print("[COMMAND] Sent to serial: " + str(command),end="")
    except serial.SerialException as e:
        print("[ERROR] Serial communication error:", e)
    except Exception as e:
        print("[ERROR] Error sending command:", e)

def receive_response():
    """
    Receive a response from STM32 via serial port.
    """
    try:
        response = ser.readline().decode('utf-8').strip()
        print("[RESPONSE] Received from serial: " + response)
        return response
    except serial.SerialException as e:
        print("[ERROR] Serial communication error:", e)
        return None
    except Exception as e:
        print("[ERROR] Error receiving response:", e)
        return None

def process_command(command):
    """
    Process a command and send it to STM32.
    """
    send_command(command)
    time.sleep(0.1)
    response = receive_response()
    return response

def get_temperature():
    """
    Get temperature from STM32.
    """
    temperature = process_command("GET_T\n")
    return temperature

def get_pressure():
    """
    Get pressure from STM32.
    """
    pressure = process_command("GET_P\n")
        # print("pressure ",pressure)
    return pressure

def set_scale(scale):
    """
    Set scale of load cell.
    """
    send_command("SET_K=" + str(scale) + "\n")
    # time.sleep(0.5)
    response = receive_response()
    print(len(response))
    if response != "SET_K=OK":
        print("[ERROR] Failed to set scale.")
        return False
    else:
        print("[INFO] Scale set successfully.")
        return True
    
def get_scale():
    """
    Get scale from STM32.
    """
    scale = process_command("GET_K\n")
    return scale
    

def get_angle():
    """
    Get angle from STM32.
    """
    angle = process_command("GET_A\n")
    return angle

def main():
    if ser.isOpen():
        print(ser.name + ' is open...')
        while True:
            try:
                # send all commands to STM32
                # process_command("GET_T\n")
                # process_command("GET_P\n")
                # time.sleep(1)
                # set_scale(123)
                get_angle()
                # process_command("GET_K\n")
                # process_command("GET_A\n")
            except KeyboardInterrupt:
                print("Exiting...")
                break

if __name__ == "__main__":
    main()

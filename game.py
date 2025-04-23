import ctypes
import numpy as np
import platform
import os

if platform.system() == "Windows":
    # Use raw string (r prefix) or double backslashes
    # lib_path = r"D:\chot_AI\setup_connect4_3\connectfour.dll"
    lib_path = "D:\\chot_AI\\setup_connect4_3\\connectfour.dll"
    lib = ctypes.CDLL(lib_path)
else:
    lib = ctypes.CDLL("./connectfour.so")  
    
lib.call_connect_four_agent.argtypes = [
    ctypes.POINTER(ctypes.c_int),  
    ctypes.c_int,                 
    ctypes.c_int                   
]
lib.call_connect_four_agent.restype = ctypes.c_int  


def get_connect_four_move(board, mark):
    if len(board) != 42:
        raise ValueError("Board must be a flat array of length 42 (6x7)")
    
    board_array = (ctypes.c_int * 42)(*board)
    move = lib.call_connect_four_agent(board_array, 42, mark)
    return move
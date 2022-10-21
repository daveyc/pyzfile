import pyzfile
from pyzfile import *

try:
    with ZFile("//'ROCKET.USER.EXEC(DTLC)'", "rb, type=record", encoding='cp1047') as file:
        print(file.info())
        print(file.amrc())
        with ZFile("//CNTL(PYZFILE)", "wb, type=record", encoding='cp1047') as out:
            for rec in file:
                out.write(rec)
                print(rec)
except Exception as error:
    print(error)

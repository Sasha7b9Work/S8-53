import os
from struct import *

def WriteByte(num_byte, file):
    value = int(symbols[num_byte])
    if value < 0:
        value += 256
    file.write(str(value))

def WriteReverseByte(num_byte, file):
    value = int(symbols[num_byte])
    if value < 0:
        value += 256
    revValue = 0
    base = 256
    while value != 0:
        revValue += (value & 1) * (base >> 1)
        base = base >> 1
        value >>= 1
    file.write(str(revValue))

def GetBit(byte, bit):
    if((byte & (1 << bit)) > 0):
        return 1
    return 0

def GetWidthByte(byte):
    retValue = 8
    for bit in range(8):
        if(GetBit(byte, bit) == 1):
            return retValue
        retValue -= 1
    return retValue

def CalculateWidth(num_symbol):
    if num_symbol >= 16 and num_symbol < 32:
        return num_symbol - 16
    if num_symbol == 32:
        return 1
    retValue = 0;
    for num_byte in range(8):
        byte = symbols[num_symbol * 8 + num_byte]
        if GetWidthByte(byte) > retValue:
            retValue = GetWidthByte(byte)
    return retValue

###### Start here ######
input = open("Font5.bin", "rb")
data = input.read()
input.close()
symbols = unpack("2048b", data)

output = open("font5.inc", "w")
output.write("#include \"font.h\"\nconst Font font5 = {\n\t5, {\n")

outputDisplay = open("font5display.inc", "w")
outputDisplay.write("const BYTE font5display[3080] = {\n")
outputDisplay.write("/* Orient = 0x00, FontID = 0x00 */ 0x00, 0x00,\n")
outputDisplay.write("/* First character ID */           0x00, 0x00,\n")
outputDisplay.write("/* Last  character ID */           0xff, 0xff,\n")
outputDisplay.write("/* Height */                       0x05, 0x00,\n")

outputInterface = open("font5.cs", "w")             # Этот шрифт будет использоваться в клиентской программе на PC
outputInterface.write("namespace ControlLibraryS8_53\n")
outputInterface.write("{\n")
outputInterface.write("\tpartial class Display\n")
outputInterface.write("\t{\n")
outputInterface.write("\t\tprivate void InitFont5()\n")
outputInterface.write("\t\t{\n")
outputInterface.write("\t\t\tfonts[0] = new MyFont();\n")
outputInterface.write("\t\t\tMyFont font = fonts[0];\n")
outputInterface.write("\t\t\tfont.height = 5;\n")

outTmpTable = open("table.tmp", "w")    # Temp file for table of symbols
outTmpMap = open("map.tmp", "w")        # Temp file for bit map

for num_symbol in range(256):
    output.write("/*" + str(num_symbol) + "*/")                         # Number of symbol
    outputInterface.write("\t\t\tfont.symbols[" + str(num_symbol) + "] = new Symbol(")
    outTmpTable.write("\n/* " + str(num_symbol) + " */\t    ")
    
    output.write("\t\t{ " + str(CalculateWidth(num_symbol)) + ", { ")   # Width symbol
    outputInterface.write(str(CalculateWidth(num_symbol)) + ", new int[8] { ")
    outTmpTable.write(str(CalculateWidth(num_symbol)) + ", ")
    offset = 8 + 4 * 256 + num_symbol * 8
    outTmpTable.write(str(offset & 0xff) + ", " + str((offset >> 8) & 0xff) + ", " + str((offset >> 16) & 0xff) + ",")

    outTmpMap.write("/* " + str(num_symbol) + " */\t        ")
    
    for num_byte in range(8):
        WriteByte(num_symbol * 8 + num_byte, output)
        WriteByte(num_symbol * 8 + num_byte, outputInterface)
        WriteReverseByte(num_symbol * 8 + num_byte, outTmpMap)
        if not(num_byte == 7 and num_symbol == 255):
            outTmpMap.write(",")
        if num_byte != 7:
           output.write(",") 
           outputInterface.write(",")
        output.write("\t")
        outputInterface.write(" ")

    outTmpMap.write("\n")
        
    output.write("} " + "}")
    outputInterface.write(" });\n")
    if num_symbol != 255:
        output.write(",")
    output.write("\t\n")
output.write("} };\n")

outTmpMap.write("};\n")

outputDisplay.write("\n/* Table of characters */\n")
outTmpTable.close()
outTmpTable = open("table.tmp")
lines = outTmpTable.readlines()
for line in lines:
    outputDisplay.write(line)

outputDisplay.write("\n\n/* Map of characters */\n\n")
outTmpMap.close();
outTmpMap = open("map.tmp")
lines = outTmpMap.readlines()
for line in lines:
    outputDisplay.write(line)

outputInterface.write("}\n")
outputInterface.write("}\n")
outputInterface.write("}\n")
outputInterface.close();

output.close()
outputDisplay.close()

input = open("font5.inc")
#print(input.read())
input.close()

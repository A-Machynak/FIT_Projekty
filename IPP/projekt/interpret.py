""" Interpreter for IPPcode22 language
Author:
    Augustin Machynak (xmachy02@stud.fit.vutbr.cz)

Examples:
    $ python3 interpret.py --source="source.xml"
    $ python3 interpret.py --input="input.xml"
"""

import sys
import xml.etree.ElementTree as ET

sys.stdin.reconfigure(encoding='utf-8')
sys.stdout.reconfigure(encoding='utf-8')

def printHelp():
    """Help message

    Note:
        Exits with error code 0 (success).
    """

    print("interpret.py")
    print("Usage:")
    print("\t python3 interpret.py [--input=[file] | --source=[file]]\n")
    print("Flags:")
    print("\t --source=[file]  -  Specify file that contains XML representation of a source file in IPPcode22")
    print("\t --input=[file]   -  Specify file to read input from")
    exit(0)

def convertOctals(str):
    """Convert octals in string (format \\xxx) to their ascii representation

    Args:
        param1 (str): String to be converted

    Returns:
        str: String with octals replaced with their ascii representation
    """

    newStr = ""
    temp = ""
    state = 0
    for i in range(0, len(str)):
        if state == 1 or state == 2: # \ or \x
            temp += str[i]
            state += 1
        elif state == 3: # \xx
            newStr += chr(int(temp + str[i]))
            temp = ""
            state = 0
            continue
        
        if str[i] == "\\":
            state = 1
        elif state == 0:
            newStr += str[i]
    return newStr

class Instruction:
    """IPPcode22 instruction.

    Attributes:
        opcode (str): Instruction opcode
        order (int): Instruction order
        arg['arg1'] (Argument): First argument
        arg['arg2'] (Argument): Second argument
        arg['arg3'] (Argument): Third argument
    """

    def __init__(self, order, opcode):
        self.opcode = opcode
        self.order = int(order)
        self.arg = {}
        self.arg['arg1'] = None
        self.arg['arg2'] = None
        self.arg['arg3'] = None

    def __str__(self):
        """Convert to string.

        Format:
            Instruction: { opcode: [opcode], order: [order], [arguments] }
        """
        txt = "Instruction: { "
        txt += "opcode: " + self.opcode + ", "
        txt += "order: " + str(self.order)
        if self.arg['arg1'] != None:
            txt += ", arg['arg1']: " + str(self.arg['arg1'])
        if self.arg['arg2'] != None:
            txt += ", arg['arg2']: " + str(self.arg['arg2'])
        if self.arg['arg3'] != None:
            txt += ", arg['arg3']: " + str(self.arg['arg3'])
        txt += " }"
        return txt

class ArgumentType:
    """Represents argument's type or variable's data type.

    Attributes:
        type(str): Type name (= nil/var/type/label/int/bool/string/float)
    """

    def __init__(self, type="nil"):
        self.type = type.lower()

    def isVar(self):
        return self.type == "var"
    def isType(self):
        return self.type == "type"
    def isLabel(self):
        return self.type == "label"
    def isInt(self):
        return self.type == "int"
    def isBool(self):
        return self.type == "bool"
    def isString(self):
        return self.type == "string"
    def isNil(self):
        return self.type == "nil" or self.type == None
    def isFloat(self):
        return self.type == "float"

    def __str__(self):
        return "ArgumentType: { " + "type: " + str(self.type) + " }"

class Argument:
    """IPPcode22 instruction argument.

    Attributes:
        argType(ArgumentType): Argument type
        argVal(any - depends on ArgumentType): Argument value
    """

    def __init__(self, argType=None, argVal=None):
        self.argType = argType
        if argVal == None:
            self.argVal = ""
        else:
            self.argVal = argVal
        
    def __str__(self):
        txt = "Argument: { "
        txt += "argType: " + str(self.argType) + ", "
        txt += "argVal: \"" + str(self.argVal) + "\" }"
        return txt

class ArgumentValueFactory:
    """Argument value factory
    """

    def create(self, type, text):
        """Argument value factory method

        Args:
            type (str): Argument type
            text (str): Argument value

        Returns:
            Class or value - based on type
        """

        if type == "var":
            # Expect text to be in format: [frame]@[variable name]
            txtSplit = text.split("@", 2)
            return VariableArg(txtSplit[1], txtSplit[0].upper())
        elif type == "label":
            return Label(text)
        elif type == "int":
            try:
                val = int(text)
            except:
                exit(32)
            return val
        elif type == "bool":
            return text.lower() == "true"
        elif type == "type":
            return ArgumentType(text)
        elif type == "string":
            if text is None:
                return ""
            return convertOctals(text)
        elif type == "float":
            return float.fromhex(text)
        else:
            return text

class Label:
    """Label

    Attributes:
        name (str): Label name
    """

    def __init__(self, name):
        self.name = name

    def __str__(self):
        return "Label: { name: \"" + self.name + "\" }"

class Value:
    """Simple class for value and type

    Attributes:
        type (ArgumentType): Type
        value (any - depends on ArgumentType): Value
    """

    def __init__(self, type, value):
        self.type = type
        self.value = value
    def __str__(self):
        return "Value: { type: " + str(self.type) + ", value: \"" + str(self.value) + "\" }"

class Variable:
    """Represents a variable

    Attributes:
        name (str): Variable name
        type (ArgumentType): Variable type
        value (any): Variable value
        initialized (bool): Whether or not this variable was assigned a value
    """
    def __init__(self, name, type=None, value=None):
        self.name = name
        if type == None:
            # Type is set to nil
            self.type = ArgumentType()
        else:
            self.type = type
        self.value = value

        if type != None or value != None:
            self.initialized = True
        else:
            self.initialized = False

    def __str__(self):
        txt = "Variable: { "
        txt += "name: \"" + str(self.name) + "\", "
        if self.initialized:
            txt += "value: \"" + str(self.value) + "\", "
            txt += "type: " + str(self.type) + " }"
        else:
            txt += "Uninitialized }"
        return txt

class VariableArg:
    """Represents variable in an argument ( [frame]@[name] ).

    Attributes:
        name (str): Variable name
        frame (str): Frame this variable belongs to (GF, TF or LF)
    """

    def __init__(self, name, frame):
        self.name = name
        self.frame = frame

    def __str__(self):
        txt = "VariableArg: { "
        txt += "name: \"" + str(self.name) + "\", "
        txt += "frame: \"" + str(self.frame) + "\" }"
        return txt

class Frame:
    """IPPcode22 frame (global/temporary/local).

    Attributes:
        vars (dict(Variable)): Variables defined in this frame.
    """

    def __init__(self):
        self.vars = {}

    def getVar(self, var):
        """Get variable from this frame

        Args:
            var(str): Variable name.

        Note:
            Exits with error code 54 if variable doesn't exist.
        """

        if var not in self.vars:
            exit(54)
        
        return self.vars[var]

    def defVar(self, var):
        """Define new variable in this frame.

        Args:
            var(str): Variable name
        
        Note:
            Exits with error code 52 if variable already exists.
        """

        if var in self.vars:
            exit(52)
        
        self.vars[var] = Variable(var)

    def move(self, var, value):
        """Move a value to variable.

        Args:
            var(str): Variable name
            value(Value): Variable type and value
        
        Note:
            Exits with error code 54 if variable doesn't exist.
        """

        if var not in self.vars:
            exit(54)
        self.vars[var].type = value.type
        self.vars[var].value = value.value
        self.vars[var].initialized = True

    def __str__(self):
        txt = "Frame variables: { "
        for var in self.vars:
            txt += "name: " + var + ", data: " + str(self.vars[var]) + ", "
        txt = txt[:len(txt)-2]
        txt += " }"
        return txt

class InterpreterState:
    """Holds IPPcode22 interpreter state.

    Attributes:
        programCounter (int): Program counter
        iExecutionCounter (int): Counter for how many instructions have been executed
        globalFrame (Frame): Global frame
        tempFrame (Frame, None): Current temporary frame
        frameStack (list(Frame)): Frame stack. Last frame is Local Frame
        callStack (list(Instruction)): Call stack (CALL, RETURN)
        dataStack (list(Value)): Data stack (POPS, PUSHS)
        instructions (dict(Instruction)): Program instructions
        labels (dict(Label)): Program labels
        highestOrder (int): Order of the last instruction

        inputFile (str, optional): String read from input file if an inputFile was specified.
    """

    def __init__(self, inputFile = None):
        self.programCounter = 0
        self.iExecutionCounter = 0

        self.globalFrame = Frame()
        self.tempFrame = None

        self.frameStack = []

        self.callStack = []
        self.dataStack = []

        self.instructions = {}
        self.labels = {}

        self.highestOrder = 0

        if inputFile == None:
            # No --input flag
            self.inputFlag = False
        else:
            self.inputFlag = True
            self.inputFileLine = 0
            f = open(inputFile, 'r', encoding="utf-8")

            # Read the whole file - so we don't leave file descriptors 
            # hanging if the program suddenly exits
            self.inputFile = f.readlines()
            f.close()

    def __str__(self):
        txt = "Interpreter state: {\n"
        txt += " Program counter: " + str(self.programCounter) + "\n"
        txt += " Instructions executed: " + str(self.iExecutionCounter) + "\n"
        txt += " Global Frame: {\n  " + str(self.globalFrame) + "\n }\n"
        txt += " Temp frame: {\n  " + str(self.tempFrame) + "\n }\n"
        txt += " Frame stack: {\n  " + str(self.frameStack) + "\n }\n"
        txt += " Call stack: {\n  " + str(self.callStack) + "\n }\n"
        txt += " Data stack: {\n  " + str(self.dataStack) + "\n }\n}"
        return txt
    
    def addInstruction(self, instr):
        """Add instruction to instruction dictionary

        Args:
            instr (Instruction): Instruction to add

        Note:
            Exits with error code 52 if instruction is a label with a name, 
            that already exists.
            Exits with error code 32 if instruction order is an invalid value
            or instruction with this order already exists.
        """

        if instr.order < 0 or instr.order in self.instructions:
            exit(32)
        self.instructions[instr.order] = instr

        if instr.opcode == "LABEL":
            labelName = instr.arg['arg1'].argVal.name
            if labelName in self.labels:
                exit(52)
            self.labels[labelName] = instr
        
        if instr.order > self.highestOrder:
            # Keep track of last instruction order
            self.highestOrder = instr.order

    def getNext(self):
        """Increase program counter and get next instruction.
        
        Returns:
            Instruction: Next instruction
            bool: False if there are no more instructions
        """

        self.programCounter += 1
        while self.programCounter not in self.instructions:
            # Keep increasing the program counter, until an instruction is found
            # or end of program is reached
            if self.programCounter > self.highestOrder:
                return False
            self.programCounter += 1

        self.iExecutionCounter += 1
        return self.instructions[self.programCounter]
    
    def getFrame(self, name):
        """Get frame using it's string representation.

        Args:
            name (str): Frame's string representation ("GF", "TF" or "LF")

        Note:
            Exits with error code 55 if local frame doesn't exist.

        Returns:
            Frame: Global, temporary or local frame
            bool: False if invalid (not "GF", "LF" or "TF") frame name was specified
        """
        if name == "GF":
            return self.globalFrame
        elif name == "TF":
            if self.tempFrame == None:
                # 55 - Frame doesn't exist. (temporary frame not initialized)
                exit(55)
            
            return self.tempFrame
        elif name == "LF":
            c = len(self.frameStack)
            if c == 0:
                # 55 - Frame doesn't exist. (read from empty frame stack)
                exit(55)
            return self.frameStack[c-1]
        return False

    
    def getInput(self):
        """Get input from stdio or file, if input flag was specified.

        Returns:
            String: Next line from input file or stdio
            Value: nil if input file reached it's last line
        """

        if self.inputFlag:
            if self.inputFileLine >= len(self.inputFile):
                return Value(ArgumentType("nil"), "nil")
            value = self.inputFile[self.inputFileLine]
            self.inputFileLine += 1
        else:
            value = input()
        if value[len(value)-1] == "\n":
            value = value[:len(value)-1]
        return value

    def getValFromSymb(self, symb, doExit=True):
        """Get symbol's value.

        Args:
            symb (Argument): Symbol to retrieve value from
            doExit (bool=True): Exit if symbol is an uninitialized variable.

        Note:
            Exits with error code 56 if symbol is an uninitialized variable
            and argument doExit is set to True.
        """

        if symb.argType.isVar():
            frame = self.getFrame(symb.argVal.frame)
            var = frame.getVar(symb.argVal.name)
            if not var.initialized:
                if doExit:
                    exit(56)
                else:
                    return None
            vType = var.type
            vValue = var.value
        else:
            vType = symb.argType
            vValue = symb.argVal
        
        if vType.isString():
            # Convert escaped characters (octal values) to ascii values
            vValue = convertOctals(vValue)
        
        return Value(vType, vValue)

    ### Beginning of IPPcode22 instructions. ###

    # MOVE <var> <symb>
    def move(self, var, symb):
        value = self.getValFromSymb(symb)
        frame = self.getFrame(var.argVal.frame)
        frame.move(var.argVal.name, value)

    # CREATEFRAME
    def createFrame(self):
        self.tempFrame = Frame()

    # PUSHFRAME
    def pushFrame(self):
        if self.tempFrame == None:
            # Missing temporary frame
            exit(55)
        
        self.frameStack.append(self.tempFrame)
        self.tempFrame = None

    # POPFRAME
    def popFrame(self):
        if len(self.frameStack) == 0:
            exit(55)
        self.tempFrame = self.frameStack.pop()

    # DEFVAR <var>
    def defVar(self, var):
        frame = self.getFrame(var.argVal.frame)
        frame.defVar(var.argVal.name)
    
    # CALL <label>
    def call(self, label):
        self.callStack.append(self.programCounter)
        if label.argVal.name not in self.labels:
            # Label doesn't exist
            exit(52)
        
        # Change program counter to the label's order
        self.programCounter = self.labels[label.argVal.name].order

    # RETURN
    def ireturn(self):
        if len(self.callStack) == 0:
            # Attempted return with empty call stack
            exit(56)
        
        self.programCounter = self.callStack.pop()

    # PUSHS <symb>
    def pushs(self, symb):
        value = self.getValFromSymb(symb)
        self.dataStack.append(value)

    # POPS <var>
    def pops(self, var):
        frame = self.getFrame(var.argVal.frame)

        if len(self.dataStack) == 0:
            # Attempted pop from empty data stack
            exit(56)
        value = self.dataStack.pop()

        # Move new value to the variable
        frame.move(var.argVal.name, value)

    # ADD/SUB/MUL/IDIV <var> <symb1> <symb2>
    def arithOp(self, aType, var, symb1, symb2):
        s1val = self.getValFromSymb(symb1)
        s2val = self.getValFromSymb(symb2)
        frame = self.getFrame(var.argVal.frame)

        # Type check (both should be type 'int' or 'float')
        if not ((s1val.type.isInt() and s2val.type.isInt()) or (s1val.type.isFloat() and s2val.type.isFloat())):
            sys.stderr.write("(@" + str(self.programCounter) + ") ADD/SUB/MUL/IDIV - incompatible types")
            exit(53)
        
        if aType == "ADD":
            value = s1val.value + s2val.value
        elif aType == "SUB":
            value = s1val.value - s2val.value
        elif aType == "MUL":
            value = s1val.value * s2val.value
        elif aType == "IDIV":
            if not s1val.type.isInt():
                sys.stderr.write("(@" + str(self.programCounter) + ") IDIV - incompatible type")
                exit(53)
            if s2val.value == 0:
                sys.stderr.write("(@" + str(self.programCounter) + ") IDIV - Attempted division by 0")
                exit(57)
            value = s1val.value // s2val.value
        elif aType == "DIV":
            if not s1val.type.isFloat():
                sys.stderr.write("(@" + str(self.programCounter) + ") DIV - Expected float")
                exit(53)
            if s2val.value == 0.0:
                sys.stderr.write("(@" + str(self.programCounter) + ") DIV - Attempted division by 0")
                exit(57)
            value = s1val.value / s2val.value
        else:
            # Internal error (for testing)
            exit(99)
        
        if s1val.type.isFloat():
            vValue = Value(ArgumentType("float"), value)
        else:
            vValue = Value(ArgumentType("int"), value)
        # Move new value to the variable
        frame.move(var.argVal.name, vValue)

    # LT/GT/EQ <var> <symb1> <symb2>
    def relOp(self, rType, var, symb1, symb2):
        s1val = self.getValFromSymb(symb1)
        s2val = self.getValFromSymb(symb2)

        frame = self.getFrame(var.argVal.frame)

        if s1val.type.isNil() or s2val.type.isNil():
            # One of the symbols is nil -> test only for equality

            if rType != "EQ":
                # LT/GT instructions cannot be used with nil
                sys.stderr.write("(@" + str(self.programCounter) + ") LT/GT instructions cannot be used with nil")
                exit(53)

            if s1val.type.isNil() and s2val.type.isNil():
                # nil == nil
                frame.move(var.argVal.name, Value(ArgumentType("bool"), True))
            else:
                # nil != other types
                frame.move(var.argVal.name, Value(ArgumentType("bool"), False))
            return
        
        # Type check (both types should be same)
        if s1val.type.type != s2val.type.type:
            sys.stderr.write("(@" + str(self.programCounter) + ") LT/GT/EQ - Types are not the same")
            exit(53)
        
        # Compare
        if rType == "LT":
            value = s1val.value < s2val.value
        elif rType == "GT":
            value = s1val.value > s2val.value
        elif rType == "EQ":
            value = s1val.value == s2val.value
        else:
            # Internal error (for testing)
            exit(99)

        # Move new value to the variable
        frame.move(var.argVal.name, Value(ArgumentType("bool"), value))

    # AND/OR <var> <symb1> <symb2>
    def boolAndOr(self, bType, var, symb1, symb2):
        s1val = self.getValFromSymb(symb1)
        s2val = self.getValFromSymb(symb2)
        frame = self.getFrame(var.argVal.frame)
        
        # Type check (both types should be 'bool')
        if not s1val.type.isBool() or not s2val.type.isBool():
            sys.stderr.write("(@" + str(self.programCounter) + ") AND/OR - Both types are not 'bool'")
            exit(53)
        
        # Compare
        if bType == "AND":
            value = s1val.value and s2val.value
        elif bType == "OR":
            value = s1val.value or s2val.value
        else:
            # Internal error (for testing)
            exit(99)

        # Move new value to the variable
        frame.move(var.argVal.name, Value(ArgumentType("bool"), value))

    # NOT <var> <symb1>
    def boolNot(self, var, symb):
        s1val = self.getValFromSymb(symb)
        frame = self.getFrame(var.argVal.frame)

        # Type check (symbol should have type 'bool')
        if not s1val.type.isBool():
            sys.stderr.write("(@" + str(self.programCounter) + ") NOT - Type is not 'bool'")
            exit(53)

        # Negate
        value = not s1val.value

        # Move new value to the variable
        frame.move(var.argVal.name, Value(ArgumentType("bool"), value))

    # INT2CHAR <var> <symb>
    def int2char(self, var, symb):
        s1val = self.getValFromSymb(symb)
        frame = self.getFrame(var.argVal.frame)

        # Type check (symbol should have type 'int')
        if not s1val.type.isInt():
            exit(53)

        # Try catch - symbol value could be too high and cause OverflowError etc.
        try:
            value = chr(s1val.value)
        except:
            # Invalid ordinal value
            exit(58)

        # Move new value to the variable
        frame.move(var.argVal.name, Value(ArgumentType("string"), value))

    # STRI2INT <var> <symb1> <symb2>
    def stri2int(self, var, symb1, symb2):
        s1val = self.getValFromSymb(symb1)
        s2val = self.getValFromSymb(symb2)
        frame = self.getFrame(var.argVal.frame)

        # Type check (symbol1 should have type 'string' and symbol2 type 'int')
        if not s1val.type.isString() or not s2val.type.isInt():
            sys.stderr.write("(@" + str(self.programCounter) + ") STRI2INT - Type check failed")
            exit(53)
        
        # Out of bounds check
        if s2val.value >= len(s1val.value) or s2val.value < 0:
            exit(58)
        
        # Get ordinal value of character at position specified by symbol2
        value = ord(s1val.value[s2val.value])

        # Move new value to the variable
        frame.move(var.argVal.name, Value(ArgumentType("int"), value))

    # READ <var> <type>
    def read(self, var, type):
        # Type check (check before reading)
        if not (type.argVal.isInt() or type.argVal.isBool() or type.argVal.isString() or type.argVal.isFloat()):
            # Invalid <type> specified - not int/string/bool/float
            sys.stderr.write("(@" + str(self.programCounter) + ") READ - Invalid type specified")
            exit(53)

        value = self.getInput()
        frame = self.getFrame(var.argVal.frame)

        if isinstance(value, Value):
            # Last line from input file was read - value is nil
            vValue = value
        elif type.argVal.isInt():
            try:
                vValue = Value(ArgumentType("int"), int(value))
            except:
                vValue = Value(ArgumentType("nil"), "nil")
        elif type.argVal.isBool():
            vValue = Value(ArgumentType("bool"), str(value).lower() == "true")
        elif type.argVal.isString():
            vValue = Value(ArgumentType("string"), convertOctals(value))
        elif type.argVal.isFloat():
            try:
                vValue = Value(ArgumentType("float"), float.fromhex(value))
            except:
                vValue = Value(ArgumentType("nil"), "nil")
        else:
            # Internal error (for testing)
            exit(99)

        # Move new value to the variable
        frame.move(var.argVal.name, vValue)

    # WRITE <symb>
    def write(self, symb):
        s1val = self.getValFromSymb(symb)

        # Type checks
        if s1val.type.isBool():
            # bool - print "true"/"false"
            if s1val.value:
                print("true", end='')
            else:
                print("false", end='')
        elif s1val.type.isNil():
            # nil - print nothing
            print("", end='')
        elif s1val.type.isFloat():
            print(float.hex(s1val.value), end='')
        else:
            # int/float/string
            print(s1val.value, end='')

    # CONCAT <var> <symb1> <symb2>
    def concat(self, var, symb1, symb2):
        s1val = self.getValFromSymb(symb1)
        s2val = self.getValFromSymb(symb2)
        frame = self.getFrame(var.argVal.frame)

        # Type checks (symbol1 and symbol2 should have type 'string')
        if not s1val.type.isString() or not s2val.type.isString():
            sys.stderr.write("(@" + str(self.programCounter) + ") CONCAT - Both types are not 'string'")
            exit(53)

        # Concatenate
        value = s1val.value + s2val.value

        # Move new value to the variable
        frame.move(var.argVal.name, Value(ArgumentType("string"), value))

    # STRLEN <var> <symb>
    def strlen(self, var, symb):
        s1val = self.getValFromSymb(symb)
        frame = self.getFrame(var.argVal.frame)

        if not s1val.type.isString():
            sys.stderr.write("(@" + str(self.programCounter) + ") STRLEN - Not a string")
            exit(53)
        value = len(s1val.value)

        # Move new value to the variable
        frame.move(var.argVal.name, Value(ArgumentType("int"), value))

    # GETCHAR <var> <symb1> <symb2>
    def getchar(self, var, symb1, symb2):
        s1val = self.getValFromSymb(symb1)
        s2val = self.getValFromSymb(symb2)
        frame = self.getFrame(var.argVal.frame)

        if not s1val.type.isString() or not s2val.type.isInt():
            sys.stderr.write("(@" + str(self.programCounter) + ") GETCHAR - Type check failed")
            exit(53)

        if s2val.value > len(s1val.value)-1 or s2val.value < 0:
            exit(58)
        value = s1val.value[s2val.value]

        # Move new value to the variable
        frame.move(var.argVal.name, Value(ArgumentType("string"), value))

    # SETCHAR <var> <symb1> <symb2>
    def setchar(self, var, symb1, symb2):
        varVal = self.getValFromSymb(var)
        s1val = self.getValFromSymb(symb1)
        s2val = self.getValFromSymb(symb2)
        frame = self.getFrame(var.argVal.frame)

        if not varVal.type.isString() or not s1val.type.isInt() or not s2val.type.isString():
            sys.stderr.write("(@" + str(self.programCounter) + ") SETCHAR - Type check failed")
            exit(53)

        if len(s2val.value) == 0 or s1val.value > len(varVal.value)-1 or s1val.value < 0:
            exit(58)

        value = varVal.value[0:s1val.value] + s2val.value[0] + varVal.value[s1val.value+1:]

        # Move new value to the variable
        frame.move(var.argVal.name, Value(ArgumentType("string"), value))

    # TYPE <var> <symb>
    def itype(self, var, symb):
        s1val = self.getValFromSymb(symb, False)
        frame = self.getFrame(var.argVal.frame)

        if s1val == None:
            # Variable is defined, but it's uninitialized
            value = ""
        else:
            value = s1val.type.type
        
        # Move new value to the variable
        frame.move(var.argVal.name, Value(ArgumentType("string"), value))

    # JUMP <label>
    def jump(self, label):
        # Set the program counter to label's order
        if label.argVal.name not in self.labels:
            exit(52)
        self.programCounter = self.labels[label.argVal.name].order

    # JUMPIFEQ/JUMPIFNEQ <label> <symb1> <symb2>
    def jumpif(self, eqneq, label, symb1, symb2):
        s1val = self.getValFromSymb(symb1)
        s2val = self.getValFromSymb(symb2)

        if label.argVal.name not in self.labels:
            exit(52)

        # Type check for nil
        if s1val.type.isNil() or s2val.type.isNil():
            if s1val.type.type != s2val.type.type:
                # nil == nil
                equal = False
            else:
                # nil != other types/values
                equal = True
        elif s1val.type.type != s2val.type.type:
            # Type check failed (different types)
            sys.stderr.write("(@" + str(self.programCounter) + ") JUMPIFEQ/JUMPIFNEQ - Type check failed")
            exit(53)
        else:
            if s1val.value == s2val.value:
                equal = True
            else:
                equal = False

        if (eqneq == "EQ" and equal) or (eqneq == "NEQ" and not equal):
            self.jump(label)

    # EXIT <symb>
    def exit(self, symb):
        var = self.getValFromSymb(symb)

        # Type check
        if not var.type.isInt():
            exit(53)

        # Value check
        if int(var.value) < 0 or int(var.value) > 49:
            exit(57)
        exit(int(var.value))

    # DPRINT <symb>
    def dprint(self, symb):
        s1val = self.getValFromSymb(symb)
        sys.stderr.write(str(s1val.value))

    # BREAK
    def ibreak(self):
        sys.stderr.write(str(self))

    # INT2FLOAT <var> <symb>
    def int2float(self, var, symb):
        s1val = self.getValFromSymb(symb)
        frame = self.getFrame(var.argVal.frame)

        if not s1val.type.isInt():
            sys.stderr.write("(@" + str(self.programCounter) + ") INT2FLOAT - Type check failed")
            exit(53)

        frame.move(var.argVal.name, Value(ArgumentType("float"), float(s1val.value)))

    # FLOAT2INT <var> <symb>
    def float2int(self, var, symb):
        s1val = self.getValFromSymb(symb)
        frame = self.getFrame(var.argVal.frame)

        if not s1val.type.isFloat():
            sys.stderr.write("(@" + str(self.programCounter) + ") FLOAT2INT - Type check failed")
            exit(53)
            
        frame.move(var.argVal.name, Value(ArgumentType("int"), int(s1val.value)))


    # STACK INSTRUCTIONS EXTENSION
    # CLEARS
    def clears(self):
        self.dataStack = []

    # ADDS/SUBS/MULS/IDIVS/DIVS
    def arithOpS(self, aType):
        if len(self.dataStack) < 2:
            exit(56)
        val2 = self.dataStack.pop()
        val1 = self.dataStack.pop()

        # Type check (both should be type 'int' or 'float')
        if not ((val1.type.isInt() and val2.type.isInt()) or (val1.type.isFloat() and val2.type.isFloat())):
            sys.stderr.write("(@" + str(self.programCounter) + ") ADDS/SUBS/MULS/IDIVS/DIVS - Type check failed")
            exit(53)
        
        if aType == "ADDS":
            value = val1.value + val2.value
        elif aType == "SUBS":
            value = val1.value - val2.value
        elif aType == "MULS":
            value = val1.value * val2.value
        elif aType == "IDIVS":
            if not val1.type.isInt():
                sys.stderr.write("(@" + str(self.programCounter) + ") IDIVS - Type check failed")
                exit(53)
            if val2.value == 0:
                # Attempted division by 0
                exit(57)
            value = val1.value // val2.value
        elif aType == "DIVS":
            if not val1.type.isFloat():
                sys.stderr.write("(@" + str(self.programCounter) + ") DIVS - Type check failed")
                exit(53)
            if val2.value == 0.0:
                # Attempted division by 0
                exit(57)
            value = val1.value / val2.value
        else:
            # Internal error (for testing)
            exit(99)
        
        if val1.type.isFloat():
            vValue = Value(ArgumentType("float"), value)
        else:
            vValue = Value(ArgumentType("int"), value)

        self.dataStack.append(vValue)
        
    # LTS/GTS/EQS
    def relOpS(self, rType):
        if len(self.dataStack) < 2:
            exit(56)
        val2 = self.dataStack.pop()
        val1 = self.dataStack.pop()

        if val1.type.isNil() or val2.type.isNil():
            # One of the symbols is nil -> test only for equality

            if rType != "EQS":
                # LT/GT instructions cannot be used with nil
                sys.stderr.write("(@" + str(self.programCounter) + ") LTS/GTS/EQS - Type check failed")
                exit(53)

            if val1.type.isNil() and val2.type.isNil():
                # nil == nil
                self.dataStack.append(Value(ArgumentType("bool"), True))
            else:
                # nil != other types
                self.dataStack.append(Value(ArgumentType("bool"), False))
            return
        
        # Type check (both types should be same)
        if val1.type.type != val2.type.type:
            sys.stderr.write("(@" + str(self.programCounter) + ") LTS/GTS/EQS - Type check failed")
            exit(53)
        
        # Compare
        if rType == "LTS":
            value = val1.value < val2.value
        elif rType == "GTS":
            value = val1.value > val2.value
        elif rType == "EQS":
            value = val1.value == val2.value
        else:
            # Internal error (for testing)
            exit(99)
        
        self.dataStack.append(Value(ArgumentType("bool"), value))

    # ANDS/ORS
    def boolAndOrS(self, bType):
        if len(self.dataStack) < 2:
            exit(56)
        val2 = self.dataStack.pop()
        val1 = self.dataStack.pop()
        
        # Type check (both types should be 'bool')
        if not val1.type.isBool() or not val2.type.isBool():
            sys.stderr.write("(@" + str(self.programCounter) + ") ANDS/ORS - Type check failed")
            exit(53)
        
        # Compare
        if bType == "ANDS":
            value = val1.value and val2.value
        elif bType == "ORS":
            value = val1.value or val2.value
        else:
            # Internal error (for testing)
            exit(99)

        self.dataStack.append(Value(ArgumentType("bool"), value))

    # NOTS
    def boolNotS(self):
        if len(self.dataStack) < 1:
            exit(56)
        val1 = self.dataStack.pop()

        # Type check (symbol should have type 'bool')
        if not val1.type.isBool():
            sys.stderr.write("(@" + str(self.programCounter) + ") NOTS - Type check failed")
            exit(53)
        # Negate
        value = not val1.value
        self.dataStack.append(Value(ArgumentType("bool"), value))

    # INT2FLOATS/FLOAT2INTS/INT2CHARS
    def convertIntFloatCharS(self, type):
        if len(self.dataStack) < 1:
            exit(56)
        val1 = self.dataStack.pop()

        if type == "INT2FLOATS":
            if not val1.type.isInt():
                sys.stderr.write("(@" + str(self.programCounter) + ") INT2FLOATS - Type check failed")
                exit(53)
            vValue = Value(ArgumentType("float"), float(val1.value))
        elif type == "FLOAT2INTS":
            if not val1.type.isFloat():
                sys.stderr.write("(@" + str(self.programCounter) + ") FLOAT2INTS - Type check failed")
                exit(53)
            vValue = Value(ArgumentType("int"), int(val1.value))
        elif type == "INT2CHARS":
            if not val1.type.isInt():
                sys.stderr.write("(@" + str(self.programCounter) + ") INT2CHARS - Type check failed")
                exit(53)
            try:
                value = chr(val1.value)
            except:
                # Invalid ordinal value
                exit(58)
            
            vValue = Value(ArgumentType("string"), value)
        else:
            # Internal error (for testing)
            exit(99)
        self.dataStack.append(vValue)

    # STRI2INTS
    def stri2ints(self):
        if len(self.dataStack) < 2:
            exit(56)
        val2 = self.dataStack.pop()
        val1 = self.dataStack.pop()

        if not val1.type.isString() or not val2.type.isInt():
            sys.stderr.write("(@" + str(self.programCounter) + ") STRI2INTS - Type check failed")
            exit(53)

        # Out of bounds check
        if val2.value >= len(val1.value) or val2.value < 0:
            exit(58)

        value = ord(val1.value[val2.value])

        self.dataStack.append(Value(ArgumentType("int"), value))

    # JUMPIFEQS/JUMPIFNEQS
    def jumpifS(self, eqneq, label):
        if len(self.dataStack) < 2:
            exit(56)
        val2 = self.dataStack.pop()
        val1 = self.dataStack.pop()

        # Type check for nil
        if val1.type.isNil() or val2.type.isNil():
            if val1.type.type != val2.type.type:
                # nil == nil
                equal = False
            else:
                # nil != other types/values
                equal = True
        elif val1.type.type != val2.type.type:
            # Type check failed (different types)
            sys.stderr.write("(@" + str(self.programCounter) + ") JUMPIFEQS/JUMPIFNEQS - Type check failed")
            exit(53)
        else:
            if val1.value == val2.value:
                equal = True
            else:
                equal = False

        if (eqneq == "EQS" and equal) or (eqneq == "NEQS" and not equal):
            self.jump(label)


# Main body
def main():
    helpFlag = False

    inputFile = None
    sourceFile = None

    # Parse arguments
    argc = len(sys.argv)
    if argc < 2:
        exit(10)
    for i in range(1, argc):
        txt = (sys.argv[i]).split("=", 2)
        if txt[0] == "--input":
            inputFile = txt[1]
        elif txt[0] == "--source":
            sourceFile = txt[1]
        elif txt[0] == "--help":
            helpFlag = True
        else:
            exit(10) # ? Unknown parameter

    # Print help
    if helpFlag:
        printHelp()
    
    if sourceFile == None and inputFile == None:
        exit(10)

    iS = InterpreterState(inputFile)
    # Parse XML
    if sourceFile == None:
        # read from stdin
        try:
            root = ET.fromstring(sys.stdin.read())
        except:
            exit(31)
    else:
        # read from file
        try:
            parser = ET.XMLParser(encoding="utf-8")
            tree = ET.parse(sourceFile, parser)
        except:
            exit(31)
        root = tree.getroot()

    # Read all the instructions
    argFactory = ArgumentValueFactory()
    if root.tag != "program" or 'language' not in root.attrib or root.attrib['language'] != "IPPcode22":
        exit(32)
    
    for child in root:
        if child.tag != "instruction" or 'order' not in child.attrib or 'opcode' not in child.attrib:
            exit(32)

        # Check if it's an actual number
        try:
            order = int(child.attrib['order'])
        except:
            exit(32)
        if order <= 0:
            exit(32)

        instr = Instruction(order, child.attrib['opcode'].upper())
        argC = 1
        for arg in child:
            if 'type' not in arg.attrib or argC > 3:
                exit(32)
            if not (arg.tag == "arg1" or arg.tag == "arg2" or arg.tag == "arg3"):
                exit(32)

            argType = arg.attrib['type'].lower()
            argValue = argFactory.create(argType, arg.text)
            instr.arg[arg.tag] = Argument(ArgumentType(argType), argValue)
            argC += 1

        if (instr.arg["arg3"] != None) and ((instr.arg["arg2"] == None) or (instr.arg["arg1"] == None)):
            exit(32)
        if (instr.arg["arg2"] != None) and (instr.arg["arg1"] == None):
            exit(32)
            
        iS.addInstruction(instr)
    
    # Start interpreting
    instr = iS.getNext()
    while instr != False:
        # Call specific function based on opcode
        if instr.opcode == "MOVE":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None:
                exit(32)
            iS.move(instr.arg['arg1'], instr.arg['arg2'])
        elif instr.opcode == "CREATEFRAME":
            iS.createFrame()
        elif instr.opcode == "PUSHFRAME":
            iS.pushFrame()
        elif instr.opcode == "POPFRAME":
            iS.popFrame()
        elif instr.opcode == "DEFVAR":
            if instr.arg['arg1'] == None:
                exit(32)
            iS.defVar(instr.arg['arg1'])
        elif instr.opcode == "CALL":
            if instr.arg['arg1'] == None:
                exit(32)
            iS.call(instr.arg['arg1'])
        elif instr.opcode == "RETURN":
            iS.ireturn()
        elif instr.opcode == "PUSHS":
            if instr.arg['arg1'] == None:
                exit(32)
            iS.pushs(instr.arg['arg1'])
        elif instr.opcode == "POPS":
            if instr.arg['arg1'] == None:
                exit(32)
            iS.pops(instr.arg['arg1'])
        elif instr.opcode == "ADD":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.arithOp("ADD", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "SUB":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.arithOp("SUB", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "MUL":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.arithOp("MUL", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "DIV":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.arithOp("DIV", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "IDIV":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.arithOp("IDIV", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "LT":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.relOp("LT", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "GT":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.relOp("GT", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "EQ":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.relOp("EQ", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "AND":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.boolAndOr("AND", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "OR":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.boolAndOr("OR", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "NOT":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None:
                exit(32)
            iS.boolNot(instr.arg['arg1'], instr.arg['arg2'])
        elif instr.opcode == "INT2CHAR":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None:
                exit(32)
            iS.int2char(instr.arg['arg1'], instr.arg['arg2'])
        elif instr.opcode == "STRI2INT":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.stri2int(instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "READ":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None:
                exit(32)
            iS.read(instr.arg['arg1'], instr.arg['arg2'])
        elif instr.opcode == "WRITE":
            if instr.arg['arg1'] == None:
                exit(32)
            iS.write(instr.arg['arg1'])
        elif instr.opcode == "CONCAT":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.concat(instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "STRLEN":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None:
                exit(32)
            iS.strlen(instr.arg['arg1'], instr.arg['arg2'])
        elif instr.opcode == "GETCHAR":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.getchar(instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "SETCHAR":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.setchar(instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "TYPE":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None:
                exit(32)
            iS.itype(instr.arg['arg1'], instr.arg['arg2'])
        elif instr.opcode == "LABEL":
            None
        elif instr.opcode == "JUMP":
            if instr.arg['arg1'] == None:
                exit(32)
            iS.jump(instr.arg['arg1'])
        elif instr.opcode == "JUMPIFEQ":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.jumpif("EQ", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "JUMPIFNEQ":
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None or instr.arg['arg3'] == None:
                exit(32)
            iS.jumpif("NEQ", instr.arg['arg1'], instr.arg['arg2'], instr.arg['arg3'])
        elif instr.opcode == "EXIT":
            if instr.arg['arg1'] == None:
                exit(32)
            iS.exit(instr.arg['arg1'])
        elif instr.opcode == "DPRINT":
            if instr.arg['arg1'] == None:
                exit(32)
            iS.dprint(instr.arg['arg1'])
        elif instr.opcode == "BREAK":
            iS.ibreak()
        elif instr.opcode == 'INT2FLOAT':
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None:
                exit(32)
            iS.int2float(instr.arg['arg1'], instr.arg['arg2'])
        elif instr.opcode == 'FLOAT2INT':
            if instr.arg['arg1'] == None or instr.arg['arg2'] == None:
                exit(32)
            iS.float2int(instr.arg['arg1'], instr.arg['arg2'])
        elif instr.opcode == "CLEARS":
            iS.clears()
        elif instr.opcode == "ADDS":
            iS.arithOpS("ADDS")
        elif instr.opcode == "SUBS":
            iS.arithOpS("SUBS")
        elif instr.opcode == "MULS":
            iS.arithOpS("MULS")
        elif instr.opcode == "DIVS":
            iS.arithOpS("DIVS")
        elif instr.opcode == "IDIVS":
            iS.arithOpS("IDIVS")
        elif instr.opcode == "LTS":
            iS.relOpS("LTS")
        elif instr.opcode == "GTS":
            iS.relOpS("GTS")
        elif instr.opcode == "EQS":
            iS.relOpS("EQS")
        elif instr.opcode == "ANDS":
            iS.boolAndOrS("ANDS")
        elif instr.opcode == "ORS":
            iS.boolAndOrS("ORS")
        elif instr.opcode == "NOTS":
            iS.boolNotS()
        elif instr.opcode == "INT2FLOATS":
            iS.convertIntFloatCharS("INT2FLOATS")
        elif instr.opcode == "FLOAT2INTS":
            iS.convertIntFloatCharS("FLOAT2INTS")
        elif instr.opcode == "INT2CHARS":
            iS.convertIntFloatCharS("INT2CHARS")
        elif instr.opcode == "STRI2INTS":
            iS.stri2ints()
        elif instr.opcode == "JUMPIFEQS":
            if instr.arg['arg1'] == None:
                exit(32)
            iS.jumpifS("EQS", instr.arg['arg1'])
        elif instr.opcode == "JUMPIFNEQS":
            if instr.arg['arg1'] == None:
                exit(32)
            iS.jumpifS("NEQS", instr.arg['arg1'])
        else:
            sys.stderr.write("Opcode " + instr.opcode + " not found\n")
            exit(32)

        instr = iS.getNext()

if __name__ == '__main__':
    main()

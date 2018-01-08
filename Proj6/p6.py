#! /usr/bin/python
import re
import sys
parsPos = 1

# dictionary of operators
operators = {}
operators["+"] = lambda a, b: a + b
operators["-"] = lambda a, b: a - b
operators["/"] = lambda a, b: a // b
operators["*"] = lambda a, b: a * b
operators["<"] = lambda a, b: a < b
operators[">"] = lambda a, b: a > b
operators["and"] = lambda a, b: True if a and b else False
operators["or"] = lambda a, b: True if a or b else False

class FunctionError(Exception):
    def __init__(self, message, *args):
        super().__init__(message, *args)
        self.message = message
    def __str__(self):
        return str(self.message)

class PrefixSyntax(Exception):
    def __init__(self, message, *args):
        super().__init__(message, *args)
        self.message = message
    def __str__(self):
        return str(self.message)

def evalOperator(function, arguments):
    ans = None
    for x in arguments:
        # check if the answer is valid
        if ans != None:
            ans = function(ans, x)
        else:
            ans = x
    return ans

def prefixEval(lis):
    global parsPos
    arguments = []
    funcName = ""
    func = None
    while parsPos < len(lis):
        x = lis[parsPos]
        if x == "(":
            parsPos += 1
            arguments.append(prefixEval(lis))
        elif x == ")":
            if len(arguments) < 2:
                raise FunctionError("Incorrect number of operands - must be at least 2 for '%s'" % funcName)
            return evalOperator(func, arguments)
        elif func == None:
            if x in operators:
                func = operators[x]
                funcName = x
            else:
                raise FunctionError("Unknown function in prefix expr: '%s'" % x)
        # at this point, it has to be an argument
        else:
            try:
                arguments.append(int(x))
            except ValueError:
                raise PrefixSyntax("Expect int found: '%s'" % x)
        parsPos += 1            
    # did not return
    raise PrefixSyntax("Missing closing ')'")

def prefixReader(file):
    global parsPos
    # looks for open or closed parentheses
    tokenizeRead = re.compile(r'([\(|\)])')
    while True:
        inputLine = file.readline() # reads one text line from the file
        # check for no input (i.e., EOF)
        if inputLine == "":
            break
        inputLine = inputLine.rstrip('\n')  # remove the newline
        print(">",inputLine)
        inputLine = tokenizeRead.sub(r" \1 ", inputLine).split()
        try:
            if inputLine[0] == "(":
                print(prefixEval(inputLine))
            else:
                raise PrefixSyntax("Missing opening '('")
        except Exception as e:
            print(e)
        finally:
            parsPos = 1
    file.close()

# open file as command line argument
file = open(sys.argv[1], "r")
prefixReader(file)

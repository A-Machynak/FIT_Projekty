package ija.projekt.uml.utils;

public class FunctionParser {

    /**
     * Parses a function in the following format:
     *  [FunctionName]([arg1]{:retVal1}, ..., [argN]{:retValN})
     *
     * @param function function in correct format
     * @param accessModifier "private"/"public"/"protected"/"package"
     * @return function declaration or null, if the format is incorrect
     */
    public static FunctionDeclaration parseFunction(String function, String accessModifier) {
        function = function.strip();

        // get the name
        int lBracketIndex = function.indexOf('(');
        int rBracketIndex = function.indexOf(')');
        if(lBracketIndex == -1 || rBracketIndex == -1
                || (lBracketIndex > rBracketIndex) || rBracketIndex != function.length()-1) {
            return null;
        }
        String name = function.substring(0, lBracketIndex);

        FunctionDeclaration fd = new FunctionDeclaration(name, accessModifier);

        if(lBracketIndex+1 == rBracketIndex) {
            return fd; // empty args
        }

        // start reading arguments
        int i = lBracketIndex+1;
        while(i != rBracketIndex) {
            // read argument
            var p1 = readArg(function, i);
            if(p1 == null) {
                return null;
            }
            i = p1.getSecond();

            // read type
            var p2 = readArgRetVal(function, i);
            if(p2 == null) {
                return null;
            }
            i = p2.getSecond();

            fd.addArgument(p1.getFirst(), p2.getFirst());
        }
        return fd;
    }

    private static Pair<String, Integer> readArg(String function, int start) {
        StringBuilder arg = new StringBuilder();
        char c;
        int i = start;

        for(; i < function.length(); i++) {
            c = function.charAt(i);
            if(c == ' ' || c == '\n') {
                continue;
            }

            if(c == ':') {
                if(arg.toString().equals("")) {
                    return null;
                }

                i++;
                break;
            }

            if(!isAlphaNumerical(c)) {
                return null;
            }
            arg.append(c);
        }

        return new Pair<>(arg.toString(), i);
    }

    private static Pair<String, Integer> readArgRetVal(String function, int start) {
        StringBuilder argRet = new StringBuilder();
        char c;
        int i = start;

        for(; i < function.length(); i++) {
            c = function.charAt(i);
            if(c == ' ' || c == '\n') {
                continue;
            }

            if(c == ',') {
                i++;
                break;
            }

            if(c == ')') {
                if(argRet.toString().equals("")) {
                    return null;
                }

                break;
            }

            if(!isAlphaNumerical(c)) {
                return null;
            }
            argRet.append(c);
        }

        return new Pair<>(argRet.toString(), i);
    }

    public static boolean isAlphaNumerical(char c) {
        return  ((int)c >= (int)'a' && (int)c <= (int)'z') ||
                ((int)c >= (int)'A' && (int)c <= (int)'Z') ||
                ((int)c >= (int)'0' && (int)c <= (int)'9');
    }
}

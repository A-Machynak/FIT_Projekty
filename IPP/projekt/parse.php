<?php
ini_set('display_errors', 'stderr');
$f = fopen('php://stdin', 'r');

enum e_types: string {
    case VAR = 'var';
    case SYMB = ''; // = e_symbType
    case TYPE = 'type';
    case LABEL = 'label';
}

enum e_symbType: string {
    case NONE = '';
    case INT = 'int';
    case BOOL = 'bool';
    case STRING = 'string';
    case NIL = 'nil';
}

/**
 * Represents a single argument in instruction
 */
class c_type {
    private int $order;         // 1/2/3
    private e_symbType $symbType;
    private e_types $type;
    private string $name;

    public function __construct(int $order, string $name, e_types $type, e_symbType $symbType) {
        $this->order = $order;
        $this->name = $name;
        $this->type = $type;
        $this->symbType = $symbType;
    }

    public function toXML(): string { // <arg{X} type="{Y}">{Z}</arg{X}>
        $problematicCharacters = array("&", "<", ">");
        $swapWith = array("&amp;", "&lt;", "&gt;");
        $newName = str_replace($problematicCharacters, $swapWith, $this->name);

        if($this->type == e_types::SYMB) {
            return "<arg" . $this->order+1 . " type=\"" . $this->symbType->value . "\">" . $newName . "</arg" . $this->order+1 . ">\n";
        }
        return "<arg" . $this->order+1 . " type=\"" . $this->type->value . "\">" . $newName . "</arg" . $this->order+1 . ">\n";
    }
}

/**
 * Represents instruction and it's argument types
 */
class c_instruction_info {
    protected string $iName; // instruction name
    protected array $types;  // e_types array
    protected int $varCount; // e_types array (expected) size

    public function __construct(string $iName, int $varCount, array $types) {
        $this->iName = $iName;
        $this->varCount = $varCount;
        $this->types = $types;

        if(sizeof($types) != $varCount) {
            fwrite(STDERR, "Internal error ($iName: $varCount expected " . sizeof($types) . ")");
            exit(23);
        }
    }

    public function getVarCount(): int {
        return $this->varCount;
    }

    public function getName(): string {
        return $this->iName;
    }

    public function toInstruction(int $order) : c_instruction {
        return new c_instruction($this->iName, $this->varCount, $this->types, $order);
    }
}

/**
 * Represents specific instruction (instance) made from c_instruction_info
 */
class c_instruction extends c_instruction_info {
    private $varArray;
    private int $currentVarCount = 0;
    private int $order;

    public function __construct(string $iName, int $varCount, array $types, int $order) {
        $this->iName = $iName;
        $this->varCount = $varCount;
        $this->types = $types;

        $this->varArray = array();
        $this->order = $order;
    }

    // Try to convert string to next expected data type
    public function convertAndAddVar(string $var) : bool {
        $split = explode("@", $var, 2);
        if($split[0] == "LF" || $split[0] == "GF" || $split[0] == "TF") {
            // VAR
            return $this->addVar($var, e_types::VAR, e_symbType::NONE);
        } else if($split[0] == "int" || $split[0] == "bool" || $split[0] == "string" || $split[0] == "nil") {
            // SYMB/TYPE
            if(sizeof($split) == 1) {
                // TYPE
                return $this->addVar($var, e_types::TYPE, e_symbType::from($split[0]));
            } else {
                // SYMB
                return $this->addVar($split[1], e_types::SYMB, e_symbType::from($split[0]));
            }
        } else {
            // LABEL
            return $this->addVar($var, e_types::LABEL, e_symbType::NONE);
        }
    }

    private function addVar(string $name, e_types $type, e_symbType $symbType) {
        if($this->currentVarCount > $this->varCount) {
            // Too many arguments or incorrect type
            return false;
        }
        if($this->types[$this->currentVarCount] == e_types::SYMB) {
            if($type != e_types::SYMB && $type != e_types::VAR) {
                // SYMB has to be either SYMB or VAR
                return false;
            }
        }

        $this->varArray[$this->currentVarCount] = new c_type($this->currentVarCount, $name, $type, $symbType);
        $this->currentVarCount++;
        return true;
    }

    public function toXML(): string {
        $result = "<instruction order=\"" . $this->order . "\" opcode=\"" . $this->iName . "\">\n";

        for($i = 0; $i < $this->currentVarCount; $i++) {
            $result = $result . $this->varArray[$i]->toXML();
        }
        return $result . "</instruction>\n";
    }
}

/**
 * Instruction definitions
 */
$instructions = array(
    'MOVE'          => new c_instruction_info('MOVE',           2, array( e_types::VAR, e_types::SYMB                   )),
    'CREATEFRAME'   => new c_instruction_info('CREATEFRAME',    0, array(                                               )),
    'PUSHFRAME'     => new c_instruction_info('PUSHFRAME',      0, array(                                               )),
    'POPFRAME'      => new c_instruction_info('POPFRAME',       0, array(                                               )),
    'DEFVAR'        => new c_instruction_info('DEFVAR',         1, array( e_types::VAR                                  )),
    'CALL'          => new c_instruction_info('CALL',           1, array( e_types::LABEL                                )),
    'RETURN'        => new c_instruction_info('RETURN',         0, array(                                               )),
    'PUSHS'         => new c_instruction_info('PUSHS',          1, array( e_types::SYMB                                 )),
    'POPS'          => new c_instruction_info('POPS',           1, array( e_types::VAR                                  )),
    'ADD'           => new c_instruction_info('ADD',            3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'SUB'           => new c_instruction_info('SUB',            3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'MUL'           => new c_instruction_info('MUL',            3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'IDIV'          => new c_instruction_info('IDIV',           3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'LT'            => new c_instruction_info('LT',             3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'GT'            => new c_instruction_info('GT',             3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'EQ'            => new c_instruction_info('EQ',             3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'AND'           => new c_instruction_info('AND',            3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'OR'            => new c_instruction_info('OR',             3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'NOT'           => new c_instruction_info('NOT',            2, array( e_types::VAR, e_types::SYMB                   )),
    'INT2CHAR'      => new c_instruction_info('INT2CHAR',       2, array( e_types::VAR, e_types::SYMB                   )),
    'STRI2INT'      => new c_instruction_info('STRI2INT',       3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'READ'          => new c_instruction_info('READ',           2, array( e_types::VAR, e_types::TYPE                   )),
    'WRITE'         => new c_instruction_info('WRITE',          1, array( e_types::SYMB                                 )),
    'CONCAT'        => new c_instruction_info('CONCAT',         3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'STRLEN'        => new c_instruction_info('STRLEN',         2, array( e_types::VAR, e_types::SYMB                   )),
    'GETCHAR'       => new c_instruction_info('GETCHAR',        3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'SETCHAR'       => new c_instruction_info('SETCHAR',        3, array( e_types::VAR, e_types::SYMB, e_types::SYMB    )),
    'TYPE'          => new c_instruction_info('TYPE',           2, array( e_types::VAR, e_types::SYMB                   )),
    'LABEL'         => new c_instruction_info('LABEL',          1, array( e_types::LABEL                                )),
    'JUMP'          => new c_instruction_info('JUMP',           1, array( e_types::LABEL                                )),
    'JUMPIFEQ'      => new c_instruction_info('JUMPIFEQ',       3, array( e_types::LABEL, e_types::SYMB, e_types::SYMB  )),
    'JUMPIFNEQ'     => new c_instruction_info('JUMPIFNEQ',      3, array( e_types::LABEL, e_types::SYMB, e_types::SYMB  )),
    'EXIT'          => new c_instruction_info('EXIT',           1, array( e_types::SYMB                                 )),
    'DPRINT'        => new c_instruction_info('DPRINT',         1, array( e_types::SYMB                                 )),
    'BREAK'         => new c_instruction_info('BREAK',          0, array(                                               ))
);


$instructionCount = 1;
$labelCount = 1;
$lineCount = 1;
$headerFound = false;
$program = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<program language=\"IPPcode22\">\n";

while($line = fgets($f)) {
    if(str_starts_with($line, "#") || $line == "\n") {
        // Comment || Empty line -> ignore
        $lineCount++;
        continue;
    } else if(!$headerFound && str_starts_with($line, ".IPPcode22")) {
        // Header check, OK
        $headerFound = true;
        $lineCount++;
        continue;
    }

    // Remove everything after comment
    $commentPos = strpos($line, "#");
    if($commentPos) {
        $removedComment = trim(substr($line, 0, $commentPos));
    } else {
        $removedComment = trim($line);
    }
    // Split by spaces
    $strArr = explode(" ", $removedComment);

    // Find the instruction
    if(array_key_exists($strArr[0], $instructions)) {
        $instr = $instructions[strtoupper($strArr[0])];

        if(!$headerFound) { // header check
            fwrite(STDERR, "Missing header");
            exit(21);
        }

        // create new instance from instruction info
        $instrInstance = $instr->toInstruction($instructionCount);
        $instructionCount++;

        // check argument count
        if(count($strArr)-1 != $instr->getVarCount()) {
            fwrite(STDERR, "Incorrect number of arguments (@$lineCount) for instruction " . $instr->getName());
            exit(23);
        }

        // for each argument
        for($i = 1; $i < count($strArr); $i++) {
            // try converting to expected type
            $ok = $instrInstance->convertAndAddVar($strArr[$i]);
            if(!$ok) {
                fwrite(STDERR, "Unknown type (@$lineCount): \"$strArr[$i]\"");
                exit(23);
            }
        }
        $program = $program . $instrInstance->toXML();
    } else {
        // expecting label
        if(str_ends_with($strArr[0], ":")) {
            // found header
            if(!$headerFound) { // header check
                fwrite(STDERR, "Missing header");
                exit(21);
            }
            $instrInstance = $instructions['LABEL']->toInstruction($instructionCount);
            $instructionCount++;

            // expect that everything before ':' is the label name
            $labelName = substr($removedComment, 0, strpos($removedComment, ':'));
            $instrInstance->convertAndAddVar($labelName);

            $program = $program . $instrInstance->toXML();
        } else {
            // unknown opcode
            fwrite(STDERR, "Unknown opcode (@$lineCount): \"$line\"");
            exit(22);
        }
    }
    $lineCount++;
}

$program = $program . "</program>\n";
print($program);

?>
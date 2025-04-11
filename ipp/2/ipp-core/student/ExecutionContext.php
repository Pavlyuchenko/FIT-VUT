<?php

namespace IPP\Student;

if (!class_exists("IPP\student\ExecutionContext")) {

class ExecutionContext {
    private array $variables = [];
    private ?SOL_Class $currentClass; // Make this nullable with the ? prefix
    private $selfObject;
    private $interpreter;
    
    public function __construct($interpreter, $self = null, ?SOL_Class $class = null) {
        $this->interpreter = $interpreter;
        $this->selfObject = $self;
        $this->currentClass = $class;
    }
    
    public function defineVariable(string $name, $value): void {
        $this->variables[$name] = $value;
    }
    
    public function getVariable(string $name) {
        if (!isset($this->variables[$name])) {
            if ($name === 'self' && $this->selfObject) {
                return $this->selfObject;
            }
            // Handle special variables like true, false, nil
            if ($name === 'true') return $this->interpreter->getTrue();
            if ($name === 'false') return $this->interpreter->getFalse();
            if ($name === 'nil') return $this->interpreter->getNil();
            
            // Error: undefined variable
            throw new \Exception("Undefined variable: $name", 32);
        }
        return $this->variables[$name];
    }
}
}

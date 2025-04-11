<?php
namespace IPP\Student;

if (!class_exists("IPP\Student\SOL_Object")) {
    class SOL_Object {
        private SOL_Class $class;
        private array $attributes = [];
        
        public function __construct(SOL_Class $class) {
            $this->class = $class;
        }
        
        public function getClass(): SOL_Class {
            return $this->class;
        }
        
        public function getAttribute(string $name) {
            if (!isset($this->attributes[$name])) {
                // Attribute not found
                return null;
            }
            return $this->attributes[$name];
        }
        
        public function setAttribute(string $name, $value): void {
            $this->attributes[$name] = $value;
        }
        
        public function __toString(): string {
            return "Instance of " . $this->class->name;
        }
    }
}

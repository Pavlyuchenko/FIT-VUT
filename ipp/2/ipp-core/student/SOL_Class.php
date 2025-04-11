<?php
namespace IPP\Student;

if (!class_exists("IPP\student\SOL_Class")) {

class SOL_Class {
	public string $name;
	public ?SOL_Class $parent;
	public array $methods;

	public function __construct(string $name, ?SOL_Class $parent = null) {
		$this->name = $name;
		$this->parent = $parent;
		$this->methods = [];
	}

	public function add_method(SOL_Method $method): void {
		array_push($this->methods, $method);
	}

	public function findMethod(string $methodName): ?SOL_Method {
		foreach ($this->methods as $method) {
			if ($method->name == $methodName) {
				return $method;
			}
		}

		return null;
	}
	
	public function __toString(): string {
		$method_names = array_map(fn($m) => $m->name, $this->methods);
		$methods_str = implode(", ", $method_names);
		$parent_name = $this->parent ? $this->parent->name : "None";

		return "Class: $this->name (Parent: $parent_name)\nMethods: [$methods_str]";
	}
}
}

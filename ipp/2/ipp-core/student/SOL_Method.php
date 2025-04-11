<?php

namespace IPP\Student;

if (!class_exists("IPP\student\SOL_Method")) {

class SOL_Method {
	public string $name;
	public bool $class_method;
	private ?\Closure $implementation;

	public function __construct(string $name, bool $class_method=false, ?\Closure $implementation = null) {
		$this->name = $name;
		$this->class_method = $class_method;
		$this->implementation = $implementation;
	}

	public function __toString(): string {
		return "Method: $this->name";
	}

	public function call(array $args) {
		return ($this->implementation)($args);
	}
}
}

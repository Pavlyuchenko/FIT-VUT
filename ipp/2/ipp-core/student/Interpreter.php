<?php

namespace IPP\Student;

use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\NotImplementedException;
use function array_push;

class SOL_Class {
	public string $name;
	public ?SOL_Class $parent;
	public array $methods;

	public function __construct(string $name, ?SOL_Class $parent = null) {
		$this->name = $name;
		$this->parent = $parent;
		$this->methods = [];
	}

	public function add_method(SOL_Method $method) {
		array_push($this->methods, $method);
	}
	
	public function __toString(): string {
		$method_names = array_map(fn($m) => $m->name, $this->methods);
		$methods_str = implode(", ", $method_names);
		$parent_name = $this->parent ? $this->parent->name : "None";

		return "Class: $this->name (Parent: $parent_name)\nMethods: [$methods_str]";
	}
}

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


function init_sol_objects(): array {
	$classes = [];

	$object = new SOL_Class("Object");
	$identicalMethod = new SOL_Method("identicalTo:", false, function($args) {
		return $args[0] + $args[1];
	});

	$object->add_method($identicalMethod);
	$object->add_method(new SOL_Method("equalTo:"));
	$object->add_method(new SOL_Method("asString"));
	$object->add_method(new SOL_Method("isNumber"));
	$object->add_method(new SOL_Method("isString"));
	$object->add_method(new SOL_Method("isBlock"));
	$object->add_method(new SOL_Method("isNil"));
	array_push($classes, $object);

	$nil = new SOL_Class("Nil", $object);
	$nil->add_method(new SOL_Method("asString"));
	array_push($classes, $nil);

	$integer = new SOL_Class("Integer", $object);
	$integer->add_method(new SOL_Method("equalTo:"));
	$integer->add_method(new SOL_Method("greaterThan:"));
	$integer->add_method(new SOL_Method("plus:"));
	$integer->add_method(new SOL_Method("minus:"));
	$integer->add_method(new SOL_Method("multiplyBy:"));
	$integer->add_method(new SOL_Method("divBy:"));
	$integer->add_method(new SOL_Method("asString"));
	$integer->add_method(new SOL_Method("asInteger"));
	$integer->add_method(new SOL_Method("timesRepeat:"));
	array_push($classes, $integer);

	$string = new SOL_Class("String", $object);
	$string->add_method(new SOL_Method("read", true));
	$string->add_method(new SOL_Method("print"));
	$string->add_method(new SOL_Method("equalTo:"));
	$string->add_method(new SOL_Method("asString"));
	$string->add_method(new SOL_Method("asInteger"));
	$string->add_method(new SOL_Method("concatenateWith:"));
	$string->add_method(new SOL_Method("startsWith:endsBefore:"));
	array_push($classes, $string);

	$block = new SOL_Class("Block");
	$block->add_method(new SOL_Method("whileTrue:"));
	array_push($classes, $block);
	
	$true = new SOL_Class("True");
	$true->add_method(new SOL_Method("not"));
	$true->add_method(new SOL_Method("and:"));
	$true->add_method(new SOL_Method("or:"));
	$true->add_method(new SOL_Method("ifTrue:ifFalse:"));
	array_push($classes, $true);
	
	$false = new SOL_Class("False");
	$false->add_method(new SOL_Method("not"));
	$false->add_method(new SOL_Method("and:"));
	$false->add_method(new SOL_Method("or:"));
	$false->add_method(new SOL_Method("ifTrue:ifFalse:"));
	array_push($classes, $false);

	return $classes;
}


class Interpreter extends AbstractInterpreter
{
	private array $classes = [];
	private array $variables = [];

    public function execute(): int
    {
        $dom = $this->source->getDOMDocument();
		$program = $dom->documentElement;

		if ($program->nodeName == "program") {
			$this->stdout->writeString("Good start\n");
		}

		$this->classes = init_sol_objects();

		// TODO: Start here, looks good
		$result = $this->classes[0]->methods[0]->call([3, 5]);
		$this->stdout->writeString("3 + 5 = $result\n");
		
		foreach ($this->classes as $classObj) {
			$this->stdout->writeString($classObj);
			$this->stdout->writeString("\n");
			$this->stdout->writeString("\n");
		}
		
		foreach ($program->getElementsByTagName("class") as $classNode) {
			$class_name = $classNode->getAttribute("name");
			$parent_name = $classNode->getAttribute("parent");
			$this->stdout->writeString("\n");
		}

		return 0;
    }
}


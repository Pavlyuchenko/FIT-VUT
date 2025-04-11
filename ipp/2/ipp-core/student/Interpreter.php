<?php

namespace IPP\Student;

use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\NotImplementedException;
use function array_push;
require_once("SOL_Class.php");
require_once("SOL_Method.php");
require_once("ExecutionContext.php");

function init_sol_objects(): array {
	$classes = [];

	$object = new SOL_Class("Object");
	$identicalMethod = new SOL_Method("identicalTo:", false, function($args) {
		return $args[0] == $args[1];
	});
	$object->add_method($identicalMethod);
	
	$equalMethod = new SOL_Method("equalTo:", false, function($args)
	{
		return $args[0] == $args[1];
	});
	$object->add_method($equalMethod);

	$asStringMethod = new SOL_Method("asString", false, function($args)
	{
		return $args[0] == $args[1];
	});
	$object->add_method($asStringMethod);

	$isNumberMethod = new SOL_Method("isNumber:", false, function($args)
	{
		return "";
	});
	$object->add_method($isNumberMethod);

	$isStringMethod = new SOL_Method("isString:", false, function($args)
	{
		return false;
	});
	$object->add_method($isStringMethod);

	$isBlockMethod = new SOL_Method("isBlock:", false, function($args)
	{
		return false;
	});
	$object->add_method($isBlockMethod);

	$isNilMethod = new SOL_Method("isNil:", false, function($args)
	{
		return false;
	});
	$object->add_method($isNilMethod);

	array_push($classes, $object);

	$nil = new SOL_Class("Nil", $object);
	$asStringMethod = new SOL_Method("asString", false, function($args)
	{
		return "nil";
	});
	$nil->add_method($asStringMethod);

	$object->add_method($isBlockMethod);
	array_push($classes, $nil);

	$integer = new SOL_Class("Integer", $object);
	$equalToMethod = new SOL_Method("equalTo:", false, function($args)
	{
		return $args[0] == $args[1];
	});
	$integer->add_method($equalToMethod);

	$greaterThanMethod = new SOL_Method("greaterThan:", false, function($args)
	{
		return $args[0] > $args[1];
	});
	$integer->add_method($greaterThanMethod);

	$plusMethod = new SOL_Method("plus:", false, function($args)
	{
		return $args[0] + $args[1];
	});
	$integer->add_method($equalToMethod);

	$method = new SOL_Method("minus:", false, function($args)
	{
		return $args[0] - $args[1];
	});
	$integer->add_method($method);

	$method = new SOL_Method("multiplyBy", false, function($args)
	{
		return $args[0] * $args[1];
	});
	$integer->add_method($method);

	$method = new SOL_Method("divBy:", false, function($args)
	{
		return $args[0] / $args[1];
	});
	$integer->add_method($method);

	$method= new SOL_Method("asString", false, function($args)
	{
		return (string) $args[0];
	});
	$integer->add_method($method);

	$method = new SOL_Method("asInteger", false, function($args)
	{
		return $args[0];
	});
	$integer->add_method($method);

	$method = new SOL_Method("timesRepeat;", false, function($args)
	{
		// TODO: Unfinished so far
		return $args[0];
	});
	$integer->add_method($method);

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

	private function findClass(string $className): ?SOL_Class 
	{
		foreach ($this->classes as $class) {
			if ($class->name === $className) {
				return $class;
			}
		}
		return null;
	}

	public function execute(): int {
		try {
			$dom = $this->source->getDOMDocument();
			$program = $dom->documentElement;

			if ($program->nodeName !== "program") {
				throw new \Exception("Invalid XML format: root element must be 'program'", 41);
			}

			// Initialize built-in classes
			$this->classes = init_sol_objects();
			
			// Parse user-defined classes from XML
			assert($program instanceof \DOMElement);
			foreach ($program->getElementsByTagName("class") as $classNode) {
				$className = $classNode->getAttribute("name");
				$parentName = $classNode->getAttribute("parent");

				$parentClass = $this->findClass($parentName);
				$newClass = new SOL_Class($className, $parentClass);

				foreach ($classNode->getElementsByTagName("method") as $methodNode) {
					$selector = $methodNode->getAttribute("selector");
					$blockNode = $methodNode->getElementsByTagName("block")->item(0);

					$method = $this->parseMethod($selector, $blockNode);
					$newClass->add_method($method);
				}
				
				// Add the new class to our collection
				$this->classes[] = $newClass;
			}
			
			// Find the Main class
			$mainClass = $this->findClass("Main");
			if (!$mainClass) {
				throw new \Exception("Missing Main class", 31);
			}
			
			// Check for run method
			$runMethod = $mainClass->findMethod("run");
			if (!$runMethod) {
				throw new \Exception("Missing run method in Main class", 31);
			}
			
			// Create a Main instance and call run
			$mainInstance = $this->createInstance($mainClass);
			$this->sendMessage($mainInstance, "run", []);
			
			return 0;
		} catch (\Exception $e) {
			$this->stderr->writeString("Error: " . $e->getMessage() . "\n");
			return $e->getCode();
		}
	}

	private function parseMethod(string $selector, \DOMElement $blockNode): SOL_Method {
		$arity = (int)$blockNode->getAttribute("arity");
		$isClassMethod = false;
		
		$implementation = function($args) use ($blockNode) {
			$context = new ExecutionContext($this);

			$parameters = $blockNode->getElementsByTagName("parameter");
			for ($i = 0; $i < $parameters->length; $i++) {
				$paramName = $parameters->item($i)->getAttribute("name");
				$context->defineVariable($paramName, $args[$i]);
			}
			
			// Execute the block's statements
			return $this->executeBlock($blockNode, $context);
		};
		
		return new SOL_Method($selector, $isClassMethod, $implementation);
	}

	private function evaluateExpression(\DOMElement $exprNode, ExecutionContext $context) {
		$children = $exprNode->childNodes;
		foreach ($children as $child) {
			if ($child->nodeType !== XML_ELEMENT_NODE) continue;
			
			switch ($child->nodeName) {
				case 'literal':
					return $this->evaluateLiteral($child);
				case 'var':
					return $context->getVariable($child->getAttribute('name'));
				case 'block':
					return $this->createBlockObject($child, $context);
				case 'send':
					return $this->evaluateSend($child, $context);
				default:
					throw new \Exception("Unknown expression type: {$child->nodeName}", 52);
			}
		}
	}

	// Execute a block
	private function executeBlock(\DOMElement $blockNode, ExecutionContext $context) 
	{
		$result = null;
		
		// Find all assignment statements in the block
		$assignNodes = [];
		foreach ($blockNode->childNodes as $child) {
			if ($child->nodeType !== XML_ELEMENT_NODE) continue;
			if ($child->nodeName === 'assign') {
				$assignNodes[] = $child;
			}
		}
		
		// Sort them by order attribute
		usort($assignNodes, function($a, $b) {
			return (int)$a->getAttribute('order') - (int)$b->getAttribute('order');
		});
		
		$this->stdout->writeString("Executing block with " . count($assignNodes) . " assignments\n");
		
		// Execute each assignment in order
		foreach ($assignNodes as $assignNode) {
			$varNode = $assignNode->getElementsByTagName('var')->item(0);
			$exprNode = $assignNode->getElementsByTagName('expr')->item(0);
			
			if (!$varNode || !$exprNode) {
				$this->stdout->writeString("Warning: Missing var or expr node in assignment\n");
				continue;
			}
			
			$varName = $varNode->getAttribute('name');
			$this->stdout->writeString("Assigning to variable: $varName\n");
			
			$value = $this->evaluateExpression($exprNode, $context);
			
			// Handle special variable '_' (ignoring its value)
			if ($varName !== '_') {
				$context->defineVariable($varName, $value);
			}
			
			// The result of the block is the value of the last expression
			$result = $value;
		}
		
		// Debug: Print all variables in the context
		$this->stdout->writeString("\n--- Variables in context after block execution ---\n");
		$variables = $context->getAllVariables(); // You'll need to add this method to ExecutionContext
		foreach ($variables as $name => $value) {
			$valueStr = $value instanceof SOL_Object 
				? "Instance of " . $value->getClass()->name
				: (string)$value;
				
			$this->stdout->writeString("$name = $valueStr\n");
			
			// If it's an object, try to get its internal value
			if ($value instanceof SOL_Object) {
				$internalValue = $value->getAttribute('value');
				if ($internalValue !== null) {
					$this->stdout->writeString("  internal value: $internalValue\n");
				}
			}
		}
		$this->stdout->writeString("----------------------------------------------\n\n");
		
		return $result;
	}

	private function evaluateSend(\DOMElement $sendNode, ExecutionContext $context) {
		$selector = $sendNode->getAttribute('selector');
		
		// Evaluate the target expression (receiver)
		$targetExpr = $sendNode->getElementsByTagName('expr')->item(0);
		$target = $this->evaluateExpression($targetExpr, $context);
		
		// Evaluate all arguments
		$args = [];
		$argNodes = $sendNode->getElementsByTagName('arg');
		foreach ($argNodes as $argNode) {
			$argExpr = $argNode->getElementsByTagName('expr')->item(0);
			$args[] = $this->evaluateExpression($argExpr, $context);
		}
		
		// Find and call the appropriate method on the target
		// This will need to handle all the SOL25 method resolution rules
		return $this->sendMessage($target, $selector, $args);
	}

	private function createInstance(SOL_Class $class): SOL_Object {
		return new SOL_Object($class);
	}

	private function sendMessage(SOL_Object $receiver, string $selector, array $args) {
		$class = $receiver->getClass();
		
		// Find the method in the class hierarchy
		$method = $this->findMethodInHierarchy($class, $selector);
		if (!$method) {
			throw new \Exception("Method not found: $selector", 51);
		}
		
		// Add the receiver as the first argument (self)
		array_unshift($args, $receiver);
		
		// Call the method
		return $method->call($args);
	}

	private function findMethodInHierarchy(SOL_Class $class, string $selector): ?SOL_Method {
		// Search in this class
		$method = $class->findMethod($selector);
		if ($method) {
			return $method;
		}
		
		// If not found and has parent, search in parent
		if ($class->parent) {
			return $this->findMethodInHierarchy($class->parent, $selector);
		}
		
		return null;
	}

	private function evaluateLiteral(\DOMElement $literalNode) {
		$class = $literalNode->getAttribute('class');
		$value = $literalNode->getAttribute('value');
		
		switch ($class) {
			case 'Integer':
				// Create Integer instance with value
				return $this->createIntegerInstance((int)$value);
			case 'String':
				// Create String instance with value
				return $this->createStringInstance($value);
			case 'True':
				return $this->getTrue();
			case 'False':
				return $this->getFalse();
			case 'Nil':
				return $this->getNil();
			case 'class':
				// Class literal
				return $this->findClass($value);
			default:
				throw new \Exception("Unknown literal class: $class", 52);
		}
	}

	private function createBlockObject(\DOMElement $blockNode, ExecutionContext $parentContext) {
		// Create a Block instance that can execute this block
		$blockClass = $this->findClass('Block');
		$blockObject = $this->createInstance($blockClass);
		
		// Store the block node and parent context for later execution
		$blockObject->setAttribute('blockNode', $blockNode);
		$blockObject->setAttribute('parentContext', $parentContext);
		
		return $blockObject;
	}

	private function createIntegerInstance(int $value): SOL_Object {
		$integerClass = $this->findClass('Integer');
		$instance = $this->createInstance($integerClass);
		$instance->setAttribute('value', $value);
		return $instance;
	}

	private function createStringInstance(string $value): SOL_Object {
		$stringClass = $this->findClass('String');
		$instance = $this->createInstance($stringClass);
		$instance->setAttribute('value', $value);
		return $instance;
	}

	private function getTrue(): SOL_Object {
		static $trueInstance = null;
		if ($trueInstance === null) {
			$trueClass = $this->findClass('True');
			$trueInstance = $this->createInstance($trueClass);
		}
		return $trueInstance;
	}

	private function getFalse(): SOL_Object {
		static $falseInstance = null;
		if ($falseInstance === null) {
			$falseClass = $this->findClass('False');
			$falseInstance = $this->createInstance($falseClass);
		}
		return $falseInstance;
	}

	private function getNil(): SOL_Object {
		static $nilInstance = null;
		if ($nilInstance === null) {
			$nilClass = $this->findClass('Nil');
			$nilInstance = $this->createInstance($nilClass);
		}
		return $nilInstance;
	}
}


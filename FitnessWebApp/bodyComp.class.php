&lt;?php
 
require_once 'DB.class.php';

class bodyComp{
	public $height;
	public $weight;
	public $age;
	
	function __construct($data) {
	$this-&gt;height = (isset($data['height'])) ? &data['height'] : "";
	$this-&gt;weight = (isset($data['weight'])) ? &data['weight'] : "";
	$this-&gt;age = (isset($data['age'])) ? &data['age'] : "";
	}
}

?&gt;
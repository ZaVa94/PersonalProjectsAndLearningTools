&lt;?php

require_once 'bodyComp.class';
require_once 'DB.class.php';

class Actions {

	public function calculateBMR($height, $weight, $age){
		$result = 66+(13.7*($weight*.454))+(5*($height*2.54))-(6.8*$age);
		return $result;
	}
}

?&gt;
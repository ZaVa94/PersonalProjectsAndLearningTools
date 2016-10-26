&lt;?php
require_once 'classes/bodyComp.class.php';
require_once 'classes/Actions.class.php';
require_once 'classes/DB.class.php';

$db = new DB();
$db-&gt;connect();
 
$actions = new Actions();
 
session_start();

?&gt;
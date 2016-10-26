&lt;?php

class DB {

	protected $db_name = 'cl_db';
    protected $db_user = 'root';
    protected $db_pass = 'mysql.default_password';
    protected $db_host = 'localhost';
	
	public function connect() {
	        $connection = mysql_connect($this-&gt;db_host, $this-&gt;db_user, $this-&gt;db_pass);
	        mysql_select_db($this-&gt;db_name);
 
	        return true;
	    }
		
	public function processRowSet($rowSet, $singleRow=false)
	    {
	        $resultArray = array();
	        while($row = mysql_fetch_assoc($rowSet))
	        {
	            array_push($resultArray, $row);
	        }
 
	        if($singleRow === true)
	            return $resultArray[0];
 
	        return $resultArray;
	    }
		
	public function select($table, $where) {
	        $sql = "SELECT * FROM $table WHERE $where";
	        $result = mysql_query($sql);
	        if(mysql_num_rows($result) == 1)
	            return $this-&gt;processRowSet($result, true);
 
	        return $this-&gt;processRowSet($result);
	    }
		
	public function update($data, $table, $where) {
	        foreach ($data as $column =&gt; $value) {
	            $sql = "UPDATE $table SET $column = $value WHERE $where";
	            mysql_query($sql) or die(mysql_error());
	        }
	        return true;
	    }

}

?&gt;
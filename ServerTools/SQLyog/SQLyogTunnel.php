<?php

/*
    SQLyog
    Copyright 2003-2006, Webyog
    http://www.webyog.com    
    
    HTTP Tunneling Page
    
    This page exposes the MySQL API as a set of web-services which is consumed by SQLyog - the most popular GUI to MySQL.
    
    This page allows SQLyog to manage MySQL even if the MySQL port is blocked or remote access to MySQL is not allowed.    
*/

/*Wrapper Functions for PHP-MYSQL and PHP-MYSQLI*/

function yog_mysql_connect ($host, $port, $username, $password, $db_name="") 
{
    $username = utf8_decode($username);    
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":
            $ret = mysql_connect ($host.':'.$port, $username, $password);
            if (strlen ($db_name)!=0){
                mysql_select_db("$db_name");
            }
            break;    
        case "mysqli":
            $port=(int)$port;
            $GLOBALS["___mysqli_ston"] = mysqli_connect($host, $username,  $password,$db_name, $port); 
            $ret=$GLOBALS["___mysqli_ston"];
            break;    
    }
    return $ret;
}


function yog_mysql_field_type ( $result, $offset ) 
{
    //Get the type of the specified field in a result
    
    $ret=0;
    switch (DB_EXTENSION) 
    {   
        case "mysql":
            $ret = mysql_field_type ( $result, $offset );
            break;    
        case "mysqli":
            $tmp=mysqli_fetch_field_direct ( $result, $offset);
            $ret = GetCorrectDataTypeMySQLI($tmp->type);
            break;     
    }
    return $ret;
}


function yog_mysql_field_len ( $result, $offset ) 
{
    //Returns the length of the specified field
    
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":
            $ret = mysql_field_len ($result,$offset );        
            break;    
        case "mysqli":
            $tmp=mysqli_fetch_field_direct ( $result, $offset);
            $ret = $tmp->length;
            break;    
    }
    return $ret;
}


function yog_mysql_field_flags ($result, $offset)
{
    //Get the flags associated with the specified field in a result
    
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":
            $ret = mysql_field_flags ($result,$offset);        
            break;    
        case "mysqli":        
            $___mysqli_obj = (mysqli_fetch_field_direct($result, $offset));
            $___mysqli_tmp = $___mysqli_obj->flags;
            $ret=($___mysqli_tmp? (string)(substr((($___mysqli_tmp & MYSQLI_NOT_NULL_FLAG)       ? "not_null "       : "") . (($___mysqli_tmp & MYSQLI_PRI_KEY_FLAG)        ? "primary_key "    : "") . (($___mysqli_tmp & MYSQLI_UNIQUE_KEY_FLAG)     ? "unique_key "     : "") . (($___mysqli_tmp & MYSQLI_MULTIPLE_KEY_FLAG)   ? "unique_key "     : "") . (($___mysqli_tmp & MYSQLI_BLOB_FLAG)           ? "blob "           : "") . (($___mysqli_tmp & MYSQLI_UNSIGNED_FLAG)       ? "unsigned "       : "") . (($___mysqli_tmp & MYSQLI_ZEROFILL_FLAG)       ? "zerofill "       : "") . (($___mysqli_tmp & 128)                        ? "binary "         : "") . (($___mysqli_tmp & 256)                        ? "enum "           : "") . (($___mysqli_tmp & MYSQLI_AUTO_INCREMENT_FLAG) ? "auto_increment " : "") . (($___mysqli_tmp & MYSQLI_TIMESTAMP_FLAG)      ? "timestamp "      : "") . (($___mysqli_tmp & MYSQLI_SET_FLAG)            ? "set "            : ""), 0, -1)) : false);                
            break;     
    }
    return $ret;
}


function yog_mysql_get_server_info ($db_link)
{
    //Get MySQL server info
    
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":
            $ret = mysql_get_server_info ($db_link);
            break;    
        case "mysqli":
            $ret = mysqli_get_server_info ($db_link);        
            break;    
    }
    return $ret;
}


function yog_mysql_affected_rows ( $db_link )
{
    //Get number of affected rows in previous MySQL operation
    
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":      
            $ret = mysql_affected_rows($db_link);        
            break;    
        case "mysqli":        
            $ret = mysqli_affected_rows($db_link);        
            break;    
    }
    return $ret;
}


function yog_mysql_insert_id ( $db_link )
{
    //Get the ID generated from the previous INSERT operation
    
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":      
            $ret = mysql_insert_id($db_link);
            break;    
        case "mysqli":       
            $ret = mysqli_insert_id($db_link);
            break;    
    }
    return $ret;
}


function yog_mysql_query ( $query, $db_link )
{
    //Send a MySQL query    
    $ret=array();
    switch (DB_EXTENSION) 
    {
        case "mysql":      
            $result = mysql_query ($query, $db_link);
            /**********************/

            if (yog_mysql_errno($db_link)!=0) {          
                $temp_ar= array("result"=>-1, "ar"=>mysql_affected_rows($db_link));
                array_push($ret, $temp_ar);                
            }              
            elseif ($result===FALSE) {
                
                $temp_ar= array("result"=>1, "ar"=>mysql_affected_rows($db_link));
                array_push($ret, $temp_ar);                                
            }
            else {
                $temp_ar= array("result"=>$result, "ar"=>mysql_affected_rows($db_link));
                array_push($ret, $temp_ar);
            }

            /**********************/            
            break;            
        case "mysqli":        
            $ret = get_array_from_query($query, $db_link);                        
            break;    
    }
    return $ret;
}

function get_array_from_query($query, $db_link) 
{
    $ret=array();
    $bool = mysqli_real_query ( $db_link, $query )or yog_mysql_error($db_link);
    
    if (yog_mysql_errno($db_link)!=0) {        

        $temp_ar= array("result"=>-1, "ar"=>0);        
        array_push($ret, $temp_ar);
        
    }
    
    elseif ($bool) {
        do {    
            /* store first result set */
            $result = mysqli_store_result($db_link);
            $num_ar= mysqli_affected_rows($db_link);

            if ($result===FALSE && yog_mysql_errno($db_link)!=0) {                          
                $temp_ar= array("result"=>-1, "ar"=>$num_ar);
                array_push($ret, $temp_ar);                
                break;
            }              
            elseif ($result===FALSE) {                
                $temp_ar= array("result"=>1, "ar"=>$num_ar);
                array_push($ret, $temp_ar);                                
            }
            else {
                $temp_ar= array("result"=>$result, "ar"=>$num_ar);
                array_push($ret, $temp_ar);
            }    
        } while (mysqli_next_result($db_link));            

        if (yog_mysql_errno($db_link)!=0) {                      
            $temp_ar= array("result"=>-1, "ar"=>$num_ar);
            array_push($ret, $temp_ar);        
        }    
    }
    return $ret;
}



function yog_mysql_errno($db_link)
{
    //Returns the numerical value of the error message from previous MySQL operation
    
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":      
            $ret = mysql_errno($db_link);
            break;    
        case "mysqli":        
            $ret = mysqli_errno($db_link);
            break;    
    }
    return $ret;
}


function yog_mysql_error($db_link)
{
    //Returns the text of the error message from previous MySQL operation
    
    $ret=0;
    switch (DB_EXTENSION)
    {
        case "mysql":      
            $ret = mysql_error ($db_link);        
            break;    
        case "mysqli":        
            $ret = mysqli_error ($db_link);        
            break;    
    }
    return $ret;
}


function yog_mysql_num_rows ( $result ) 
{
    //Get number of rows in result
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":      
            $ret = mysql_num_rows ($result);        
            break;    
        case "mysqli":        
            $ret = mysqli_num_rows ($result);        
            break;    
    }
    return $ret;
}


function yog_mysql_num_fields ( $result )
{
    //Get number of fields in result
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":      
            $ret = mysql_num_fields ($result);        
            break;    
        case "mysqli":       
            $ret = mysqli_num_fields ($result);
            break;      
    }
    return $ret;
}


function yog_mysql_fetch_field($result)
{
    //Get column information from a result and return as an object
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":       
            $ret = mysql_fetch_field ($result);
            break;    
        case "mysqli":
            $ret = mysqli_fetch_field ($result);
            break;    
    }
    return $ret;
}


function yog_mysql_fetch_array ( $result)
{
    //Fetch a result row as an associative array, a numeric array, or both
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":
            $ret=mysql_fetch_array ( $result);        
            break;    
        case "mysqli":
            $ret=mysqli_fetch_array ( $result);        
            break;    
    }
    return $ret;
}


function yog_mysql_fetch_lengths ( $result )
{
    //Get the length of each output in a result
    $ret=array();
    switch (DB_EXTENSION) 
    {
        case "mysql":
            $ret = mysql_fetch_lengths ($result);        
            break;    
        case "mysqli":
            $ret = mysqli_fetch_lengths ($result);
            break;    
    }
    return $ret;
}


function yog_mysql_free_result ( $result )
{
    //Free result memory
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":
            $ret = mysql_free_result ($result);
            break;    
        case "mysqli":
            $ret = mysqli_free_result ($result);
            break;    
    }
    return $ret;
}


function yog_mysql_select_db ( $db_name, $db_link )
{
    //Select a MySQL database
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":
            $ret = mysql_select_db ($db_name, $db_link);
            break;    
        case "mysqli":
            $ret = mysqli_select_db ($db_link, $db_name);        
            break;    
    }
    return $ret;
}


function yog_mysql_close ( $db_link )
{
    //Close MySQL connection
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysql":
            $ret = mysql_close ( $db_link );
            break;    
        case "mysqli":
            $ret = mysqli_close ( $db_link );
            break;    
    }
    return $ret;
}

function yog_mysql_connect_errno()
{
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysqli":
            $ret=mysqli_connect_errno ();
            break;
        case "mysql":
            $ret=mysql_errno ();
            break;
    }
    return $ret;
}


function yog_mysql_connect_error() 
{
    $ret=0;
    switch (DB_EXTENSION) 
    {
        case "mysqli":
            $ret=mysqli_connect_error ();
            break;
        case "mysql":
            $ret=mysql_error ();
            break;
    }
    return $ret;
}



function GetCorrectDataTypeMySQLI( $type ) 
{
    switch($type)
    {           
        case MYSQLI_TYPE_TINY:          
        	$data = "tinyint";
            break;
        case MYSQLI_TYPE_SHORT:
        	$data = "int";
            break;
        case MYSQLI_TYPE_LONG:         
        	$data = "int";
            break;
        case MYSQLI_TYPE_FLOAT:          
        	$data = "float";
            break;
        case MYSQLI_TYPE_DOUBLE:          
        	$data = "double";
            break;
        case MYSQLI_TYPE_NULL:          
        	$data = "default null";
            break;
        case MYSQLI_TYPE_TIMESTAMP:          
        	$data = "timestamp" ;
            break;
        case MYSQLI_TYPE_BIT:          
        	$data = "bit" ;
            break;
        case MYSQLI_TYPE_LONGLONG:          
        	$data = "bigint";
            break;
        case MYSQLI_TYPE_INT24:          
        	$data = "mediumint";
            break;
        case MYSQLI_TYPE_DATE:          
        	$data = "date";
            break;
        case MYSQLI_TYPE_TIME:          
        	$data = "time";
            break;
        case MYSQLI_TYPE_DATETIME:          
        	$data = "datetime";
            break;
        case MYSQLI_TYPE_YEAR:          
        	$data = "year";
            break;
        case MYSQLI_TYPE_NEWDATE:          
        	$data = "date";
            break;
        case MYSQLI_TYPE_ENUM:          
        	$data = "enum";
            break;
        case MYSQLI_TYPE_SET:          
        	$data = "set";
            break;
        case MYSQLI_TYPE_TINY_BLOB:          
        	$data = "tinyblob";
            break;
        case MYSQLI_TYPE_MEDIUM_BLOB:          
        	$data = "mediumblob";
            break;
        case MYSQLI_TYPE_LONG_BLOB:          
        	$data = "longblob";
            break;
        case MYSQLI_TYPE_BLOB:                      
            $data = "blob";           
            break;
        case MYSQLI_TYPE_VAR_STRING:          
        	$data = "varchar";
            break;
        case MYSQLI_TYPE_STRING:          
        	$data = "char";
            break;
        case MYSQLI_TYPE_GEOMETRY:          
        	$data = "geometry";
            break;

	    }    
    return ($data);
}
/* function finds and returns the correct type understood by MySQL C API() */

function GetCorrectDataType ( $result, $j )
{
    $data   = NULL;

    WriteLog ( "Enter GetCorrectDataType" );

    switch( yog_mysql_field_type ( $result, $j ) )
    {
        case "int":
            if ( yog_mysql_field_len ( $result, $j ) <= 4 )
            {
                $data = "smallint";
            }
            elseif ( yog_mysql_field_len ( $result, $j ) <= 9 )
            {
                $data = "mediumint";
            }
            else
            {
                $data = "int";
            }
            break;
    
        case "real":
            if (yog_mysql_field_len($result,$j) <= 10 )
            {
                $data = "float";                                             
            }
            else
            {
                $data = "double";
            }
            break;

        case "string":
            $data = "varchar";
            break;

        case "blob":
            $textblob = "TEXT";
            if ( strpos ( yog_mysql_field_flags ($result,$j),"binary") )
            {
                $textblob = "BLOB";
            }
            if (yog_mysql_field_len($result,$j) <= 255)
            {
                if ( $textblob == "TEXT" )
                {
                    $data = "tinytext";
                }
                else
                {
                    $data = "tinyblob";
                }
            }
            elseif (yog_mysql_field_len($result, $j) <= 65535 )
            {
                if ( $textblob == "TEXT" ) {
                    $data = "mediumtext";
                }
                else
                {
                    $data = "mediumblob";
                }
            }
            else
            {
                if ( $textblob == "TEXT" ) {
                    $data = "longtext";
                }
                else
                {
                    $data = "longblob"; 
                }
            }
            break;

        case "date":
            $data = "date";
            break;

        case "time":
            $data = "time";
            break;

        case "datetime":
            $data = "datetime";
            break;
    }

    WriteLog ( "Exit GetCorrectDataType" );

    return (convertxmlchars($data));
}

/* Output extra info used by SQLyog internally */

function HandleExtraInfo ( $mysql, $value )
{

    WriteLog ( "Enter HandleExtraInfo" );

    echo "<s_v>" . yog_mysql_get_server_info ( $mysql ) . "</s_v>";
    echo "<m_i></m_i>";
    echo "<a_r>" . $value['ar'] . "</a_r>";
    echo "<i_i>" . yog_mysql_insert_id ( $mysql ) . "</i_i>";

    WriteLog ( "Exit HandleExtraInfo" );

}


/* Process when only a single query is called. */

function ExecuteSingleQuery ( $mysql, $query )
{
    WriteLog ( "Enter ExecuteSingleQuery" );     
    
    $result     = yog_mysql_query ( $query, $mysql );   
    foreach ($result as $key => $value)
    {
        //$value['result'],$value['ar'];

        if ( $value['result']=== -1 ) {
            HandleError ( yog_mysql_errno($mysql), yog_mysql_error($mysql) );
            return;
        }
    
        /* free the result */
        CreateXMLFromResult ($mysql, $value);

        if ($value['result']!==1) {
            yog_mysql_free_result ( $value['result'] );
        }        
    }       

    WriteLog ( "Exit ExecuteSingleQuery" );
}


function CreateXMLFromResult ($mysql, $value)
{
    // $value['result'], $value['ar']
    /* query execute was successful so we need to echo the correct xml */
    /* the query may or may not return any result */
    WriteLog ( "yog_mysql_num_rows in ExecuteSingleQuery" );

    // check if the query is not a result returning query    
    $isNotResultQuery=0;
    if ( DB_EXTENSION=="mysqli") {        
        ($value['result'] ===1)?$isNotResultQuery=1:$isNotResultQuery=0;
    }   
    else{        
        ($value['result'] ==1)?$isNotResultQuery=1:$isNotResultQuery=0;
    }    

    $numrows=0;
    $numfields=0;

    if (!is_int($value['result'])) {
        $numrows=yog_mysql_num_rows ( $value['result'] );
        $numfields=yog_mysql_num_fields ( $value['result'] );
    }   
    
    if ( $isNotResultQuery  || (!$numrows && !$numfields ))//
    {   
        /* is a non-result query */
        echo "<result v=\"" . tunnelversion . "\">";
        echo "<e_i></e_i>";        
        HandleExtraInfo ( $mysql, $value );
        echo "<f_i c=\"0\"></f_i><r_i></r_i></result>";
        return;
    }

    /* handle result query like SELECT,SHOW,EXPLAIN or DESCRIBE */
    echo '<result v="' . tunnelversion . '">';
    echo "<e_i></e_i>";
    
    /* add some extra info */    
    HandleExtraInfo ( $mysql, $value );

    /* add the field count information */
    $fieldcount     = yog_mysql_num_fields ( $value['result'] );
    print ( $fieldcount );
    echo "<f_i c=\"$fieldcount\">";

    /* retrieve information about each fields */
    $i = 0;
    while ($i < $fieldcount ) 
    {
        $meta = yog_mysql_fetch_field($value['result']);

        echo "<f>";
        echo "<n>" . convertxmlchars($meta->name) . "</n>";
        echo "<t>" . convertxmlchars($meta->table) . "</t>";
        echo "<m>" . convertxmlchars($meta->max_length) . "</m>";
        echo "<d></d>";        
        switch (DB_EXTENSION) 
        {
        case "mysql":
            echo "<ty>" . GetCorrectDataType ( $value['result'], $i ) . "</ty>";
            break;
        case "mysqli":
            echo "<ty>" . yog_mysql_field_type( $value['result'], $i ) . "</ty>";
            break;
        }
        
        echo "</f>";

        $i++;
    }

    /* end field informations */
    echo "</f_i>";

    /* get information about number of rows in the resultset */    

    echo "<r_i c=\"$numrows\">";
    /* add up each row information */
    while ( $row = yog_mysql_fetch_array ( $value['result'] ) )
    {
        $lengths = yog_mysql_fetch_lengths ( $value['result'] );

        /* start of a row */
        echo "<r>";

        for ( $i=0; $i < $fieldcount; $i++ ) 
        {
            /* start of a col */
            echo "<c l=\"$lengths[$i]\">"; 
            if ( !isset($row[$i]) /*== NULL*/ ) 
            {
                echo "(NULL)";
            }
            else 
            {
                  if ( $lengths[$i] == 0 )
                    {
                        echo "_";
                    }
                    else
                    {
                        echo convertxmlchars ( base64_encode ( $row[$i] ) );
                    }
            }

            /* end of a col */
            echo "</c>";
        }

        /* end of a row */
        echo "</r>";
    }

    /* close the xml output */
    echo "</r_i></result>";
}


/* implementation of my_strtok() in PHP */

/*
 * Description: string my_strtok(string $string, string $delimiter).
 *
 * Function my_strtok() splits a string ($string) into smaller
 * strings (tokens), with each token being delimited by the delimiter
 * string ($delimiter), considering string variables and comments
 * in the $string argument. Note that the comparision is case-insensitive.
 *
 * Returns FALSE if there are no tokens left.
 * Does not return empty tokens.
 * Does not return the "delimiter" command as a token.
 *
 * Usage:
 * The first call to my_strtok() uses the $string and $delimiter arguments.
 * Every subsequent call to my_strtok() needs no arguments at all, or only
 * the $delimiter argument to use, as it keeps track of where it is in the
 * current string. To start over, or to tokenize a new string you simply
 * call my_strtok() with the both arguments again to initialize it.
 * The delimiter can be changed by the command "delimiter new_delimiter" in
 * the $string argument (the command is case-insensitive).
 *
 * Example:
 *  $res = my_strtok($query, $delimiter);
 *  while ($res) {
 *      echo "token = $res<br>";
 *      $res = my_strtok();
 *  }
 *
 * Author: Andrey Adaikin, IVA Team, <IVATeam@gmail.com>
 * @version $Revision: 1.3 $, $Date: 2005/09/28 $
 */

function my_strtok($string = NULL, $delimiter = NULL) {

    static $str;            // lower case $string (equals to strtolower($string))
    static $str_original;   // stores $string argument
    static $len;            // length of the $string
    static $curr_pos;       // current position in the $string
    static $match_pos;      // position where the $delimiter is a substring of the $string
    static $delim;          // lower case $delimiter (equals to strtolower($delimiter))

    WriteLog ( "Enter my_strtok" );

    if (NULL === $delimiter) 
    {
        if (NULL !== $string) 
        {
            $delim = strtolower($string);
            $match_pos = -1;
        }
    } 
    else 
    {
        if (!is_string($string) || !is_string($delimiter)) 
        {
            return FALSE;
        }
        $str_original = $string;
        $str = strtolower($str_original);
        $len = strlen($str);
        $curr_pos = 0;
        $match_pos = -1;
        $delim = strtolower($delimiter);
    }

    if ($curr_pos >= $len) 
    {
        return FALSE;
    }

    if ("" == $delim) 
    {
        $delim = ";";
        $match_pos = -1;
    }

    $dlen = strlen($delim);
    $result = FALSE;

    for ($i = $curr_pos; $i < $len; ++$i) 
    {
        if ($match_pos < $i) 
        {
            $match_pos = strpos($str, $delim, $i);
            if (FALSE === $match_pos) 
            {
                $match_pos = $len;
            }
        }

        if ($i == $match_pos) 
        {
            if ($i != $curr_pos) 
            {
                $result = trim(substr($str_original, $curr_pos, $i - $curr_pos));
                if (strncasecmp($result, 'delimiter', 9) == 0 && (strlen($result) == 9 || FALSE !== strpos(" \t", $result{9}))) 
                {
                    $delim = trim(strtolower(substr($result, 10)));
                    if ("" == $delim) { $delim = ";"; }
                    $match_pos = -1;
                    $result = FALSE;
                }
            }
            $i += $dlen;
            if ($match_pos < 0) 
            {
                $dlen = strlen($delim);
            }
            $curr_pos = $i--;
            if ("" === $result) 
            {
                $result = FALSE;
            }
            if (FALSE !== $result) 
            {
                break;
            }
        } else if ($str{$i} == "'") {
            for ($j = $i+1; $j < $len; ++$j) {
                if ($str{$j} == "\\") ++$j;
                else if ($str{$j} == "'") break;
            }
            $i = $j;
        } else if ($str{$i} == "\"") {
            for ($j = $i+1; $j < $len; ++$j) {
                if ($str{$j} == "\\") ++$j;
                else if ($str{$j} == "\"") break;
            }
            $i = $j;
        } else if ($i < $len-1 && $str{$i} == "/" && $str{$i+1} == "*") {
            $j = $i+2;
            while ($j) {
                $j = strpos($str, "*/", $j);
                if (!$j || $str{$j-1} != "\\") { break; }
                ++$j;
            }
            if (!$j) { break; }
            $i = $j+1;
        } else if ($str{$i} == "#") {
            $j = strpos($str, "\n", $i+1) or strpos($str, "\r", $i+1);
            if (!$j) { break; }
            $i = $j;
        } else if ($i < $len-2 && $str{$i} == "-" && $str{$i+1} == "-" && FALSE !== strpos(" \t", $str{$i+2})) {
            $j = strpos($str, "\n", $i+3) or strpos($str, "\r", $i+1);
            if (!$j) { break; }
            $i = $j;
        } else if ($str{$i} == "\\") {
            ++$i;
        }
    }

    if (FALSE === $result && $curr_pos < $len) {
        $result = trim(substr($str_original, $curr_pos));
        if (strncasecmp($result, 'delimiter', 9) == 0 && (strlen($result) == 9 || FALSE !== strpos(" \t", $result{9}))) {
            $delim = trim(strtolower(substr($result, 10)));
            if ("" == $delim) { $delim = ";"; }
            $match_pos = -1;
            $dlen = strlen($delim);
            $result = FALSE;
        }
        $curr_pos = $len;
        if ("" === $result) {
            $result = FALSE;
        }
    }

    WriteLog ( "Exit my_strtok" );

    return $result;
}


/* Processes a set of queries. The queries are delimited with ;. Will return result for the last query only. */
/* If it encounters any error in between will return error values for that query */

function ExecuteBatchQuery ( $mysql, $query )
{

    WriteLog ( "Enter ExecuteBatchQuery" );

    $prev  = FALSE;
    $token = my_strtok ( $query, ";" );

    while ( $token )
    {
        $prev = $token;

        $token = my_strtok();

        if ( !$token )
        {
            return ExecuteSingleQuery ( $mysql, $prev );
        }

        $result = yog_mysql_query ( $prev, $mysql );
        
        foreach ($result as $key => $value)
        {
            //$value['result'], $value['ar']
            if ( $value['result']===-1 ) {                
                HandleError ( yog_mysql_errno($mysql), yog_mysql_error($mysql) );
                return;
            }
            
            /* free the result */            
            if (!is_int($value['result'])) {
                yog_mysql_free_result ( $value['result'] );
            }
            
        }
    }

    WriteLog ( "Exit ExecuteBatchQuery" );

    return;
}


/* Function sets the MySQL server to non-strict mode as SQLyog is designed to work in non-strict mode */
function SetNonStrictMode ( $mysql )
{

    WriteLog ( "Enter SetNonStrictMode" );

    /* like SQLyog app we dont check the MySQL version. We just execute the statement and ignore the error if any */
    $query = "set sql_mode=''";
    $result = yog_mysql_query ( $query, $mysql );
       
    WriteLog ( "Exit SetNonStrictMode" );

    return;
}


/* Starting from SQLyog v5.1, we dont take the charset info from the server, instead SQLyog send the info 
   in the posted XML */

function SetName ( $mysql )
{

    global      $charset;

    WriteLog ( "Enter SetName" );

    if ( $charset === NULL ) 
        return;

    $query = 'SET NAMES ' . $charset;
    if ($charset!="[default]") {        
        yog_mysql_query ( $query, $mysql );        
    }    

    WriteLog ( "Exit SetName" );

    return;
}

/* Start element handler for the parser */

function startElement ( $parser, $name, $attrs )
{
    global  $xml_state;
    global  $host;
    global  $port;
    global  $db;
    global  $username;
    global  $pwd;
    global  $batch;
    global  $query;
    global  $base;
    global  $libxml2_is_base64;

    WriteLog ( "Enter startelement" );

    //Done for bug in PHP 5.2.6 and libXML 2.7,2. 
    // Special HTML characters were being dropped. So, now we provide to send always as base 64 encoded data.

    $base = 0;
    if(isset($attrs['E']))
    {
        $base  = (( $attrs['E'] == '1' )?(1):(0)); 
    }
    
    switch ( strtolower ( $name ) ) 
    {

    case "host":
        $xml_state  = XML_HOST;        
        break;
    case "db":
        $xml_state  = XML_DB;
        break;
    case "charset":
        WriteLog ( 'Got charset' );
        $xml_state  = XML_CHARSET;
        break;
    case "user":
        $xml_state  = XML_USER;
        break;
    case "password":
        $xml_state  = XML_PWD;
        break;
    case "port":
        $xml_state  = XML_PORT;
        //This is an assumption that, if port is sent as base64 encoded
        //the "Always Use Base64 Encoding For Data Stream is checked."
        if($base == 1)
        {
            $libxml2_is_base64 = 1;
        }
        break;
    case "query":
        $xml_state  = XML_QUERY;
        /* track whether the query(s) has to be processed in batch mode */
        $batch = (( $attrs['B'] == '1' )?(1):(0));
        break;
    case "libxml2_test_query":
        $xml_state  = XML_LIBXML2_TEST_QUERY;
        break;
    }
    WriteLog ( "Exit startelement" );
}

/* End element handler for the XML parser */

function endElement ( $parser, $name )
{
    WriteLog ( "Enter endElement" );
    
    global $xml_state;
    
    $xml_state  =   XML_NOSTATE;

    WriteLog ( "Exit  endElement" );
}

/* Character data handler for the parser */

function charHandler ( $parser, $data )
{
    
    global  $xml_state;
    global  $host;
    global  $port;
    global  $db;
    global  $username;
    global  $pwd;
    global  $batch;
    global  $query;
    global  $base;
    global  $charset;
    global  $libxml2_test_query;

    WriteLog ( "Enter charhandler" );

    //Done for bug in PHP 5.2.6 and libXML 2.7,2. 
    // Special HTML characters were being dropped. So, now we provide to send always as base 64 encoded data.
    if ( $base ) 
    {
        $data      = base64_decode ( $data );
    }

    if ( $xml_state == XML_HOST ) 
    {
        $host       .= $data;
    }
    else if ( $xml_state == XML_DB ) 
    {
        $db         .= $data;
    }
    else if ( $xml_state == XML_CHARSET ) 
    {
        $charset    .= $data;
    }
    else if ( $xml_state == XML_USER ) 
    {
        $username   .= $data;
    }
    else if ( $xml_state == XML_PWD )
    {
        $pwd        .= $data;
    }
    else if ( $xml_state == XML_PORT )
    {
        $port       .= $data;
    }
    else if ( $xml_state == XML_QUERY )
    {
        $query      .= $data;        
    }
    else if ( $xml_state == XML_LIBXML2_TEST_QUERY )
    {
        $libxml2_test_query .= $data;        
    }

    WriteLog ( "Exit charhandler" );
}


/* Convert special characters such as <,>,&, /,\, to equivalent xmlor html characters*/
function convertxmlchars ( $string,$called_by="" )  
{   
    WriteLog ( "Enter convertxmlchars, called by".$called_by );
    WriteLog ( "Input: " . $string );
    
    $result = $string;   
    
    $result = eregi_replace('&', '&amp;', $result);  
    $result = eregi_replace('<', '&lt;', $result);   
    $result = eregi_replace('>', '&gt;', $result);   
    $result = eregi_replace('\'', '&apos;', $result);
    $result = eregi_replace('\"', '&quot;', $result);

    WriteLog ( "Output: " . $result );
    WriteLog ( "Exit convertxmlchars" );
 
    return $result;  
}

/* Function writes down the correct XML for handling mysql_pconnect() error */

function HandleError ( $errno, $error )
{
    
    WriteLog ( "Enter handleerror" );
    
    echo "<result v=\"" . tunnelversion . "\"><e_i><e_n>$errno</e_n><e_d>" . convertxmlchars($error) . "</e_d></e_i></result>";

    WriteLog ( "Exit handleerror" );
}


/* Parses the XML received and stores information into the variables passed as parameter */

function GetDetailsFromPostedXML ( $xmlrecvd )
{

    WriteLog ( "Enter getdetailsfrompostedxml" );

    $xml_parser     = xml_parser_create ();
    xml_set_element_handler($xml_parser, "startElement", "endElement");
    xml_set_character_data_handler ( $xml_parser, "charHandler" );

    WriteLog ("Starting to parse:");
    WriteLog($xmlrecvd);
    $ret = xml_parse ( $xml_parser, $xmlrecvd );   
    

    if ( !$ret ) 
    {
        HandleError ( xml_get_error_code ( $xml_parser ), xml_error_string ( xml_get_error_code ( $xml_parser ) ) );
        return FALSE;
    }

    xml_parser_free($xml_parser);

    WriteLog ( "Exit getdetailsfrompostedxml" );

    return TRUE;
}


/* we dont allow an user to connect directly to this page from a browser. It can only be accessed using SQLyog */

function ShowAccessError ( $extra = "" )
{   

    WriteLog ( "Enter showaccesserror" );

    $errmsg  = '<div align="justify"><p><b>Tunnel version: ' . tunnelversion . '</b>.<p>This PHP page exposes the MySQL API as a set of webservices.<br><br>This page allows SQLyog to manage a MySQL server even if the MySQL port is blocked or remote access to MySQL is not allowed.<br><br>Visit <a href ="http://www.webyog.com">Webyog</a> to get more details about SQLyog.';

    echo ( '<html><head><title>SQLyog HTTP Tunneling</title></head>'.           
           '<body leftmargin="0" topmargin="0">'.           
           '<p align="center"><img src="http://www.webyog.com/images/header.jpg" alt="Webyog"><p>' 
         );
    echo ( '<table align="center" width="60%" cellpadding="3" border="0">'.
           '<tr><td><font face="Verdana" size="2">' . $errmsg . 
           '</td</tr></table>' );

    /* we show PHP version error also if required */
    if ( CheckPHPVersion() == FALSE ) {
        echo ( '<table width="100%" cellpadding="3" border="0"><tr><td><font face="Verdana" size="2"><p><b>Error: </b>SQLyog HTTP Tunnel feature requires PHP version > 4.3.0</td></tr></table>' );
    }

    if ($extra != "" ) {
        echo ( '<table width="100%" cellpadding="3" border="0"><tr><td><font face="Verdana" size="2"><p><b>Error!' . $extra . '</td></tr></table>' );
    }

    echo ( '</body></html>' );

    WriteLog ( "Exit showaccesserror" );
}


/* Check PHP version compatibility*/
function CheckPHPVersion()
{

    $phpversionstr  = phpversion();
    $versionarry    = explode(".", $phpversionstr, 2);

    /* We dont support v4.3.0 */
    if ( (integer)$versionarry[0] < 4 || 
         ((integer)$versionarry[0] == 4 && (integer)$versionarry[1] < 3 ))
    {
        return FALSE;        
    } 
    else
    {
        return TRUE;
    }

    return TRUE;
}




/* function checks if a required module is installed or not */

function AreModulesInstalled ()
{
    WriteLog ( "Enter aremodulesinstalled" );

    $modules        = get_loaded_extensions();
    $modulenotfound = '';

    $thisExtension="";
    if ( extension_loaded  ( "xml" ) != TRUE ) 
    {
        $modulenotfound = 'XML';
        $thisExtension="this extension";
    } else if ( DB_EXTENSION=="-1") 
    {
        $modulenotfound = 'php_mysqli or php_mysql';  
        $thisExtension="one of these these extensions";
    } 
    else 
    {
        WriteLog ( "Exit aremodulesinstalled" );
        // if correct modules are found - exit IMMEDIATELY        
        return TRUE;
    }
    
    if(isset($_GET['app'])) // from SQLyog - just indicate to SQLyog that SOME modules are not installed
    {
        echo(tunnelversionstring);
        echo(phpmoduleerror);
        WriteLog ( "Exit aremodulesinstalled" );
        return FALSE;
    }

    // It will come here only when its called from a browser
    $errmsg   = '<font color="#FF0000"><b>Error:</b> </font>Extension <b>' . $modulenotfound . '</b> was not found compiled and loaded in the PHP interpreter. SQLyog requires '.$thisExtension .' to work properly.';   
    $errmsg  .= '<p><b>Tunnel version: ' . tunnelversion . '</b>.<p>This PHP page exposes the MySQL API as a set of webservices.<br><br>This page allows SQLyog to manage a MySQL server even if the MySQL port is blocked or remote access to MySQL is not allowed.<br><br>Visit <a href ="http://www.webyog.com">Webyog</a> to get more details about SQLyog.';

    echo ( '<html><head><title>SQLyog HTTP Tunneling</title></head><body leftmargin="0" topmargin="0"><div align=center><img src="http://www.webyog.com/images/header.jpg" alt="Webyog"><p>' );
    echo ( '<table align=center width="60%" cellpadding="3" border="0"><tr><td><font face="Verdana" size="2">' . $errmsg . '</td</tr></table>' );
    echo ( '</body></html>' );

    WriteLog ( "Exit aremodulesinstalled" );
}


/**
 * checks if the XML stream is base 64 encoded
 *
 * @return bool If stream is encoded, return true. Else false;
 *
 */
function LibXml2IsBase64Encoded()
{
    global $libxml2_is_base64;
    if ($libxml2_is_base64 == 0){
        return false;
    }
    return true;
}

/** Detect if the user's PHP/LibXML is affected by the following bug: -
 *  http://bugs.php.net/bug.php?id=45996 
*/
function LibXml2IsBuggy()
{    
    global $libxml2_test_query;
    $libxml2_test_query = '';
    
    $testxml = '<xml><libxml2_test_query>select &apos;a&apos;</libxml2_test_query></xml>';
    
    GetDetailsFromPostedXML ($testxml);
    
    if(strcasecmp($libxml2_test_query, 'select a') == 0)
    {
        //This PHP/LibXML is buggy!
        return true;
    }
    
    //Not buggy!
    return false;
}


/* we can now use SQLyogTunnel.php to log debug informations, which will help us to point out the error */
function WriteLog ( $loginfo )
{
    if ( defined("DEBUG") ) 
    {
        $fp = fopen ( "yogtunnel.log", "a" );

        if ( $fp == FALSE )
            return;

        fwrite ( $fp, $loginfo . "\r\n" ); // MY_CHANGE: Because below it was 
                                          // printing 'Enter' instead of new line        
        fclose ( $fp );
    }
}

function WriteLogTemp ( $loginfo )
{
    if ( defined("DEBUG") ) 
    {
        $fp = fopen ( "sabya.log", "a" );

        if ( $fp == FALSE )
            return;

        fwrite ( $fp, $loginfo . "\r\n" ); // MY_CHANGE: Because below it was 
                                           // printing 'Enter' instead of new line        
        fclose ( $fp );
    }
}

/* Process the  query*/
function ProcessQuery ()
{ 
    WriteLog ( "Enter processquery" );
    
    if ( CheckPHPVersion() == FALSE ) 
    {
        /*  now the call can be of three types
            1.) Specific to check tunnel version
            2.) Normal where it is expected that the PHP page is 4.3.0
            3.) From browser
            
            We check this by checking the query string which is sent if just a check is done by SQLyog */

        WriteLog ( "CheckPHPVersion is FALSE" );        
        if(isset($_GET['app']))
        {            
            echo(tunnelversionstring);
            echo(phpversionerror);
        }
        else
        {
            WriteLog ( "CheckPHPVersion is FALSE and app query string not set" );
            ShowAccessError();
        }

        return;
            
    }
    
    /* in special case, sqlyog just sends garbage data with query string to check for tunnel version. we need to process that now */
    if(isset($_GET['app']))
    {
        WriteLog ( "app query string not set" );
        echo(tunnelversionstring);
        echo(tunnelversion);
        return;
    }

    /* Starting from 5.1 BETA 4, we dont get the data as URL encoded POST data, we just get it as raw data */
    WriteLog ( "Trying to get php://input" );    
    $xmlrecvd = file_get_contents("php://input");
    WriteLog ( "Got php://input!" );
    
    if ( strlen ( $xmlrecvd ) == 0 ) 
    {
        WriteLog ( "Called from browser." );
        ShowAccessError ();
        WriteLog ( "xmlrecvd is blank" );       
        return;
    }
    
    WriteLog ( $xmlrecvd );

    global  $host;
    global  $port;
    global  $username;
    global  $pwd;
    global  $db;
    global  $batch;
    global  $query;
    global  $base;

    $ret = GetDetailsFromPostedXML ( $xmlrecvd );

    if ( $ret == FALSE )
        return;

    /* Wrap result in <XML> tag */
    echo "<xml>";

    //If the stream is not base-64 encoded and the PHP has the libxml2 bug display extra error.    
    if(LibXml2IsBase64Encoded() == false && LibXml2IsBuggy() == true)
    {
        $extra_msg =
            'Your PHP/libxml version is affected by a bug. ' .
            'Please check "Always Use Base64 Encoding For Data Stream" in "Advanced" section of HTTP tab.'
        ;
        HandleError (4, $extra_msg);
        WriteLog ('libxml2 error');
        echo "</xml>";
        return;
    }    

    /* connect to the mysql server */
    WriteLog ( "Trying to connect" );
    $mysql      = yog_mysql_connect ( $host,$port, $username, $pwd );
    if ( !$mysql )
    {
        HandleError ( yog_mysql_connect_errno(), yog_mysql_connect_error() );
        WriteLog ( yog_mysql_connect_error() );
        echo "</xml>";
        return;
    }

    WriteLog ( "Connected" );    

    /* Function will execute setnames in the server as it does in SQLyog client */
    SetName ( $mysql );

    yog_mysql_select_db ( str_replace ( '`', '', $db ), $mysql );

    /* set sql_mode to zero */
    SetNonStrictMode ( $mysql );

    if ( $batch ) {
        ExecuteBatchQuery ( $mysql, $query );
    }
    else 
        ExecuteSingleQuery ( $mysql, $query );

    yog_mysql_close ( $mysql );
    
    echo "</xml>";

    WriteLog ( "Exit processquery" );
}



/*********** Global Section **********/

define ( "COMMENT_OFF", 0 );
define ( "COMMENT_HASH", 1 );
define ( "COMMENT_DASH", 2 );
define ( "COMMENT_START", 0 );

/* current element state while parsing XML received as post */

define ( "XML_NOSTATE", 0 );
define ( "XML_HOST", 1 );
define ( "XML_USER", 2 );
define ( "XML_DB", 3 );
define ( "XML_PWD", 4 );
define ( "XML_PORT", 5 );
define ( "XML_QUERY", 6 );
define ( "XML_CHARSET", 7 );

define ( "XML_LIBXML2_TEST_QUERY", 8);
/* uncomment this line to create a debug log */
//define ( "DEBUG", 1 );

/* version constant */
/* You will need to change the version in processquery method too, where it shows: $versionheader = 'TunnelVersion:5.13.1' */

define ( "tunnelversion", '8.05');
define ( "tunnelversionstring", 'TunnelVersion:' );
define ( "phpversionerror", 'PHP_VERSION_ERROR' );
define ( "phpmoduleerror", 'PHP_MODULE_NOT_INSTALLED' );

if (!defined("MYSQLI_TYPE_BIT")) {
    define ( "MYSQLI_TYPE_BIT", 16);
}

/* current character in the query */
$curpos         = 0;

//ini_set("display_errors",0);

/* check whether global variables are registered or not */
if (!get_cfg_var("register_globals")) 
{ 
    extract($_REQUEST);
}

/* we have to set the value to be off during runtime coz it does not work when magic_quotes_runtime = On is setin Php.ini */
set_magic_quotes_runtime (0);


/* Check for the PHP_MYSQL/PHP_MYSQLI extension loaded */

if (extension_loaded('mysqli')) 
{
    define ("DB_EXTENSION", "mysqli");
}
elseif (extension_loaded('mysql')) 
{
    define ("DB_EXTENSION", "mysql");
}
else 
{
    define ("DB_EXTENSION", "-1");
}




/* global variable to keep the state of current XML element */
$xml_state      = XML_NOSTATE;

/* global variables to track various informations about the query */

$host           = NULL;
$port           = NULL;
$db             = NULL;
$username       = NULL;
$pwd            = NULL;                                          
$charset        = NULL;
$batch          = 0;
$base           = 0;
$query          = NULL;
$libxml2_test_query  = NULL;
$libxml2_is_base64 = 0;


/* we stop all error reporting as we check for all sort of errors */
WriteLog ( "" );
if ( defined("DEBUG") ) 
    error_reporting ( E_ALL );
else
    error_reporting ( 0 );

set_time_limit ( 0 );


/* we check if all the external libraries support i.e. expat and mysql in our case is built in or not */
/* if any of the libraries are not found then we show a warning and exit */

if ( AreModulesInstalled () == TRUE ) 
{    
    
    ProcessQuery ();
    
}
?>

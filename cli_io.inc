<?php

##########################################################################
#									                                     #
# NOTES:								                                 #
#  1)  Anywhere I've done things that the user may want changed, I've    #
#      used a row of four hashes ("####") to mark the location.          #
#									                                     #
#  1a) parseFlags:  by default, this is run automatically at the end of  #
#      the constructor in the argHandler class, but that can be changed  #
#      by commenting out a single line;  look for "####parseFlags".      #
#									                                     #
#  1b) Null Flags:  in most cases, defining a flag as "name=1s", for     #
#      example, will require an argument after the flag.  However, I've  #
#      chosen to make testing for existence of the following argument    #
#      the responsibility of whoever uses this library.  Therefore, if   #
#      no argument is specified, the flag will be given a value of       #
#      "undefined ".  You can change this behaviour quite		         # 
#      easily, either by commenting out three lines to get an error 	 #
#      message instead, or by changing what is inserted.  To find it,    #
#      search for "####nullFlags".  To see this behaviour, try using     #
#      demo-argHandler as follows:					                     #
#      $ ./demo-argHandler -v						                     #
#									                                     #
##########################################################################

//cl_io.inc

function read() 
 { # Begin function read()
   # I built this before I found a reference to fread:  really, that's  probably
   # a better way to do this.  Still, this will will work if you'd rather use it.
   # One advantage to this, over using fgets or fread directly, is that it will 
   # automatically strip out the trailing newline characters from your input.
   # also, it's easier to type. 
   $fp=fopen("/dev/stdin", "r");
   $input=fgets($fp, 255);
   fclose($fp);
   
   return str_replace("\n","",$input);
 } # End function read()

class argHandler
 { # Begin class argHandler
   # See the end of the class for documentation.
   
   # First, define the variables on the argHandler object
   
    var $cli_flags;		# flags from the command line
    var $num_flags;	# number of flags from the command line
   
   # the next two variables, $flag_defs() and $valid_flags, are used to
   # define what flags are allowed.  $flag_defs[] contains the original list,
   # as entered by the programmer, with the flag name and the name of the variable
   # that should be used.  This includes the flag modifiers, so we can figure out 
   # later whether to look for strings.  flag[] just contains the variable name
   # and its value. 
    var $flag_defs;
    var $flag;
   
   # I'm also including an array of bad args -- it can be set up to return
   # whatever error the user wants.
   
    var $bad_flags;
   
   # no_flags is set to 1 if there are no flags specified on the command line.
    var $no_flags;
   
   # this is a var that gets reset regularly -- it's just used to see if the
   # variable that was just checked was good.
   
    var $flag_was_good = "0";
   
   # argument is used in a fairly rare occurence, where the flag is given in the 
   # form --flag=arg
   var $argument;
   
   # firstarg is only really used if there's a broken or bad argument given.
   
   var $firstarg;
   
   # This first function is a constructor, and sets up some basic variables for
   # us, without user intervention.
   
   function argHandler ()
    { # Begin function argHandler
      $this->cli_flags = $_SERVER['argv'];	# flags from the command line
      $this->num_flags = $_SERVER['argc'];	# number of flags from the command line
   
      # Since PHP considers the name of the file called to be a command line
      # argument, it's important to get rid of that:  So, we cut off the first
      # item in the array, and subtract it from the total number of flags.
      
      $this->cli_flags = array_slice($this->cli_flags, 1);
      $this->num_flags--;
      
      # If num_flags now equals zero (that is, nothing was entered on the command-
      # line), set no_flags to zero.
      
      if ($this->num_flags == 0) { $this->no_flags = 1; }
    } # End function argHandler
   
   # First, let's build a very basic test function, to be sure we're taking
   # input correctly.
   
    function argTester()
     {
       // Correct English grammar...
      $this->num_flags > 1 || $this->num_flags == 0 ? $plural = 's' : $plural = '';
      
      // Display the number of arguments received
      fwrite(STDOUT,"Got $this->num_flags argument$plural\n");
      
      // Write out the contents of the $argv array
      foreach ( $this->cli_flags as $key => $value ) 
       {
       fwrite(STDOUT,"$key => $value\n");
       }
    } # End function argTester
   
    
   # The next function, setValidFlags(), is used to create the actual listings
   # the program uses to decide whether a flag is allowed.  Interpretation
   # comes later.
   
    function setValidFlags()
     {
      $numargs = func_num_args();	# Figures out how many arguments were passed
   				                    # to setValidFlags()
   
      # The next thing to do is to start parsing.  What we want to do is to 
      # create a 2D array, with the name of the variable for the flag on
      # one side, and the default value on the other side.  In all cases,
      # the default value is "null",
   
      $i = 0;	# Set a counter
      while ($i < $numargs)
       { # Begin while loop
   
         # Build the two arrays, flag[] and flag_defs[].
         $vflag = func_get_arg($i);
         $flag_var = func_get_arg($i+1);
         
         # flag_defs[] is exactly what was put in here, seperated into a string-
         # indexed array.
         $this->flag_defs["$vflag"] = $flag_var;
         # flag[] is the variable we want created.  Default value for every variable
         # is "null"
         $this->flag["$flag_var"] = null;
   
         $i = $i+2;
   
       } # End while loop
   ####parseFlags
   # Comment the following line if you want to use parseFlags as a 
   # separate function:
    $this->parseFlags();
     } # End setValidFlags
    
   #  The parseFlags() function works with the actual command line arguments;
   #  basically, it takes each argument in turn, and either sets the value in
   #  $this->flag[], or throws an error.  This is (slightly) more complicated 
   #  than it sounds, because some flags have further arguments, and some
   #  single entries may contain more than one flag (ie, "ls -al"). 
   
     function parseFlags()
     {
      $i = 0;
     
      while ( $i < $this->num_flags )
       {
        # 1) check to see if $cli_flags[$i] is a valid flag, or a set of valid
        #    flags.
   
        # 2) check whether the flag has further arguments
        #    if yes, go on to the next step.
        #    if no, skip to the last step.
   
        # 2a) Set the value of the matching flag in $flag[] to whatever the
        #    next CLI arg is.  Shift two steps right.
   
        # 3) Set the value of the matching flag in $flag[] to 1, unless it was
        #    set in step 3.  Shift one step right. 
   
        # In order to do this, there are a few steps.
        # First, define what a short flag and long flags look like.
   
        $regexp_for_short = "/^-\w+/";  # That is, a short flag is in the form
   				     # of a single hyphen, plus one or more
                                        # word characters.
   
        $regexp_for_long = "/^--\w+/";   # A long flag is a double-hyphen, plus
   				     # one or more word characters
   
        # It is possible that a flag will be entered in the format -flag=arg.
        # If that is the case, it will break some of the regular expressions.
        # Therefore, we need to split it apart before we get to the expressions.
        $regexp_for_eqarg = "/^-{1,2}\w+=\w+/";
        if ( preg_match($regexp_for_eqarg, $this->cli_flags[$i]) )
        {
         $full = explode("=", $this->cli_flags[$i]);
         $this->argument = $full["1"];
         $this->cli_flags[$i] = $full["0"];
        }
   
        # Let's look first at short flags, which will probably be the majority.
        
        if ( preg_match($regexp_for_short, $this->cli_flags[$i]) )
         {
          # figure out how long the list is.
          $strlen = strlen($this->cli_flags[$i]);
   
          for ( $j = 1; $j < $strlen; $j++)
           {
            # Look just at the character we're testing right now
   	 $this->firstarg = substr($this->cli_flags[$i], $j, 1);
   
            $reg = "/^$this->firstarg\w*".'[=]?[\d]?[^!]$/'; # Match: $this->firstarg followed by 0+
   					   # word chars, not ending with "!"
   
            # Now, check against $this->flag_defs[]
   	 while ( list($index,) = each ($this->flag_defs) )
            {
             # If there is a match, do some stuff
   	  if ( preg_match($reg, $index) )
             {
   	   # $variable holds the name of the variable associated with the flag
   	   $variable = $this->flag_defs[$index];
   
              # Check whether there should be another arg, and deal with it.
              # In the case where the flag is in the form "--flag=arg", go with
              # the first option:  $this->argument was set earlier.
              # In the case where it is in the form "--flag arg", use the second
              # case.  In the second case, $i gets incremented one extra time,
              # so that the next flag is read properly.
              $reg_for_extra_arg = '/(=\w+)$/';
              if ( preg_match($reg_for_extra_arg, $index) ) 
              {
               # The flag will have 1 - 9 arguments.  In order to account for 
               # the possiblity of more than one, we need to run a quick test.
               
               # First, set $n (the number of args) to a default of 1.
               $numargs = 1;
               
               # Now, check to see if the flag has been defined as having more
               # than one arg, and reset $n if necessary.
   
               if ( preg_match('/[2-9]/', substr($index, -2, 1)) )
               {
                $numargs = substr($index, -2, 1);
               }
               if ($this->argument)
               {
                $this->flag[$variable] = $this->argument;
               }
               else
               {
                for ($j = 1; $j <= $numargs; $j++)
                { 
                 if ($j > 1) { $this->flag[$variable] = $this->flag[$variable].", "; }
   ####nullFlags
   # WARNING!  This next statement guarantees that all flags will register.
   # If they aren't given a value on the command line, they are automatically
   # assigned a value of NULL;  this is enough to keep scripts from crashing
   # on a blank required variable, which may not be desirable.  If not, comment
   # out the next four lines.
   
   	      if (!isset($this->cli_flags[$i+$j]))
   	       {
             $this->cli_flags[$i+$j] = "undefined";
           }
   
          $this->flag[$variable] = $this->flag[$variable].$this->cli_flags[$i+$j];
           }
                $i = $i + $numargs;
               }
              } # End extra arg statments
              else
              {
               $this->flag[$variable] = "1";
              }
              $this->flag_was_good = "1";
             } # End if statement
            } # End while
            # Reset the current element in the flag_defs array to the beginning.
   	 # If this doesn't happen, only the first flag will be matched properly!
   
            if ( $this->flag_was_good == "0" )
            {
   	  $count = count($this->bad_flags);
             $this->bad_flags[$count] = "$this->firstarg";
            }
   
            reset($this->flag_defs);
           } # End for loop
         } # End if for short variables
   
         # Short variables being done, it's time to look at the long ones.
         
   # This is basically the same as the section for short flags, except that the
   # regular expressions change
   
        if ( preg_match($regexp_for_long, $this->cli_flags[$i]) )
         {
          # Now we know it's a long variable, so we need to try to match
          # to something in flag_defs
          
          # The first step is to lose the double-hyphen:
          $flag_len = strlen($this->cli_flags[$i]) - 2;
          $this->current_flag = substr($this->cli_flags[$i], 2, $flag_len);
   
          # Now, set up the regular expression:  Acceptable matches are:
   	# $current_flag
           # $current_flag=s
   	# $current_flag=ns where "n" is a number between 2 and 9, inclusive
   	# $current_flag!
          $reg = "/^$this->current_flag((=([2-9]?)s)?|!?)$/";
          
          # Start iterating through the flag_defs array
          while ( list($index,) = each ($this->flag_defs) )
          { 
           # if there's a match, use it:
           if ( preg_match($reg, $index) )
   	{
   	 # $variable holds the name of the variable associated with the flag
              $variable = $this->flag_defs[$index];
              # Check whether there should be another arg, and deal with it.
              # In the case where the flag is in the form "--flag=arg", go with
              # the first option:  $this->argument was set earlier.
              # In the case where it is in the form "--flag arg", use the second
              # case.  In the second case, $i gets incremented one extra time,
              # so that the next flag is read properly.
              $reg_for_extra_arg = '/(=\w+)$/';
              if ( preg_match($reg_for_extra_arg, $index) )
              {
               $numargs = 1;
               if ( preg_match('/[2-9]/', substr($index, -2, 1)) )
                {
                 $numargs = substr($index, -2, 1);
                }
   
               if ( $this->argument)
               {
                $this->flag[$variable] = $this->argument;
               }
               else
               {
                for ($j = 1; $j <= $numargs; $j++)
                {
                 if ($j > 1) { $this->flag[$variable] = $this->flag[$variable].", "; }
                 $this->flag[$variable] = $this->flag[$variable].$this->cli_flags[$i+$j];
                }
                $i = $i + $numargs;
   
   #             $this->flag[$variable] = $this->cli_flags[$i+1];
   #             $i++;
               }
              }
              else
              {
               $this->flag[$variable] = "1";
              }
              $this->flag_was_good = "1";
           } # End if statement
          } # End while
   
   
          if ( $this->flag_was_good == "0" )
           {
   	 $count = count($this->bad_flags);
            $this->bad_flags[$count] = "$this->current_flag";
           }
   
   
          # Reset the current element in the flag_defs array to the beginning.
          # If this isn't done, only the first flag will be matched properly.
          reset($this->flag_defs);       
   
         } # End if for long variables
   
       # Don't forget to add one to the counter!
       $i++;
       } # End while loop
   
     } # End parseFlags
   
   
   ##
   # Function list:
   
   # argHandler() -- the constructer.  I'd like to make this call everything
   #                 else, if I can figure out how.
   #
   # setValidFlags() -- takes a list of valid flags and what the variable
   # 		     they link to should be called.
   #
   # parseFlags() -- takes no input, but uses the values set earlier.
   #		  This does the vast majority of the work in this object.
   /*
   Purpose:
    Basically, I wanted to be able to use semi-complicated command line flags,
   and I didn't like any of the existing options.  What I really wanted was 
   something like perl's getOptions, but it didn't seem to exist for PHP. 
   Well, now I know why.  It was a pain to write.  Not hard, precisely, but 
   a pain.  The goal of this project was to be able to use a variety of 
   different types of command line flags, with or without further 
   arguments, and have a single, easy way to define them.  The types I most 
   wanted were:
   
   1) A single flag.  Something like --help or -h.
   
   2) A single flag, which had to be given in the long form:  that is to 
   say, --absolute would work, but -a wouldn't.
   
   3) A flag with one further argument.  For instance, "--flag value" or 
   "-f value".
   
   4) A flag with multiple arguments.  For instance, "--twoargflag arg1 
   arg2" or "-t arg1 arg2".  For practical purposes, I limited this to a 
   maximum of nine further arguments, on the grounds that I couldn't think 
   of a use for more than two, so nine gave a good safety.
   
   As I mentioned earlier, I also wanted a quick, easy way to define the 
   valid flags, and I liked the way perl's getOptions worked when I used 
   that.  So, I needed a way to make a two-column listing, with the first 
   column being the name and limiters, and the second column being the 
   variable name to use.  I eventually decided on the following limiting 
   format:
   
   setValidFlags(
   "flag1", "variable1",      	<-- defines a single flag.
   "flag2!", "variable2",		<-- defines an absolute. (See 2, above)
   "flag3=s", "variable3",		<-- defines a flag with one further 
   				    argument.  (See 3, above) 
   "flag4=2s", "variable4");	<-- defines a flag with 2 arguments.  
   				    The 2 can be replaced with any digit 
   				    up to 9.  (See 4, above)
   
   Unfortunately, it isn't currently possible to combine types:  that may 
   be added later, but for the moment you can only use one at a time.
   
   So the next question is, why build an object?  Why not just build a 
   bunch of functions?  Well... basically, because I wanted to learn how 
   PHP deals with objects, and this seemed like a good opportunity.
   
   Usage:
   
     Use of the object was intended to be simple, and mostly is.
   (Below this explanation is a piece of sample code you can copy and 
   modify, if you're still not quite sure what to do with this)
   
   1) Make sure you're including the cli_io object:
   
     include "./cli_io.inc";
   
   2) Create an instance of the class:
   
     $arg = new argHandler;
   
   3) Set the list of valid flags:
   
   $arg->setValidFlags(
   "aflag", "vara",
   "bflag!", "varb",
   "cflag=s", "varc",
   "dflag=2s", "vard");
   
   4) Write out the rest of your program!  Flags are accessed as follows 
   (based on step 3, above):
   
   Normal flag: (ie, "-aflag", "-a")
   $arg->flag["vara"]  <-- will be null if not specified, 1 if specified.
   
   Flag with one argument:  (ie, "--cflag arg1", "-c arg1", and so on.)
   $arg->flag["varc"]  <-- will be null if not specified, or will contain 
   			arg1 if specified.
   
   Flag with multiple arguments: (ie, "--dflag arg1 arg2" or "-d arg1 
   				arg2")
   $arg->flag["vard"]  <-- contains a comma separated list of the 
                           arguments, or is null.
   
   Special cases:
   
   $arg->no_flags 	    <-- is set to 1 if no flags are specified on the 
   			command line.
   
   $arg->bad_flags     <-- contains an array of rejected flags.
   
   For a demonstration of how to use these, see demo-argHandler.php, which 
   should have been included with this library.
   
   I will, at some point, write more formal documentation for this, but at 
   the moment use demo-argHandler.php as both a demo and documentation.
   
   */
} # End class argHandler

?>


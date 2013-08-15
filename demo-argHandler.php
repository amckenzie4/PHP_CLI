#!/usr/bin/php -q

<?php
# I'm building this as a demonstration of the arghandler object.
# Eventually, the arghandler library (arghandler.php) will be rolled
# into the php_io.inc file, but I want to get it working first.
# UPDATE: (8 August, 2006) argHandler has now been rolled into 
# php_io.inc.  This demo has been updated to follow the change.

# First, include the file.
include "./cli_io.inc";

# The first thing to do is to create an instance of the object.
$arg = new argHandler();

# Next, I'm going to use $arg->argtester() to demonstrate that it has
# found the command line arguments, if any.

echo "Output from \$arg->argtester():\n";

$arg->argtester();

# Once we've done that, we go on to the meat of the program.  But first,
# let's print a seperator:

echo "\n\n---------------------\n\n";
echo "Beginning program output:";
echo "\n-------------------------\n\n";

#  In a real program, the next step would be to define acceptable flags.
# This is done using setValidFlags(), in the following format:
#
# setValidFlags(
# "flag1", "flag1_variable_name",
# "flag2", "flag2_variable_name"
# );

#  It should be possible to use any number of flags, at least in theory.
#
# SPECIAL CHARACTERS:
# There are a few reserved characters in use.  
# If you end the name of a 
# flag definition with the string "=s", the library will use the next entry
# as the name for that variable.  For instance:
# "flag3=s", "flag3_variable_name",
# 
# If you end the name of a flag definition with "=Ns", where N is a 
# digit from 2 to 9, the library will set the variable to a 
# comma-separated list of the following N arguments.
# For instance: 
# "flag3=2s", "flag3_multi_variable_name",
#
# If you end the name of a variable with an exclamation point, the library
# will NOT match any short form.  For instance,
# "flag4!", "flag4_variable_name"
#
# At the moment, it isn't possible to combine special flag types.

$arg->setValidFlags(
"help", "use_message",		# standard flag
"clear!", "screen_clear",	# standard flag, no short form
"value=s", "value",		# flag with single argument
"message", "message",		# standard flag
"twovars=2s", "multivar",	# flag with two arguments
"first", "first",
"second", "second"
);

# If you want to see the layout of the flag_defs property, 
# uncomment the next three lines.

#echo "-----------Looking at set valid flags----------\n";
#print_r($arg->flag_defs);
#echo "\n----------------done-------------------------\n";

# OK, so the list of valid flags is defined, and the system knows about
# the command line arguments.  The next step is to combine those two 
# pieces of information, and set up variables.  So, we use parseFlags() to 
# match the command-line information to the definitions from setValidFlags().
# UPDATE: (8 August, 2006) This step is no longer necessary.  The 
# parseFlags() function still exists, but is now called automatically by
# setValidFlags().  This behaviour can be changed in the cli_io.inc 
# file, by commenting out line 129 (or search for the phrase "####", 
# which denotes that section of code).

#$arg->parseFlags();

# To see the list of available variables, with their values,
# uncomment the next three lines.

#echo "\n---------Looking at available variables------\n";
#print_r($arg->flag);
#echo "\n----------------done-------------------------\n";

# Once parseFlags() has done its thing, any flags on the command line
# that match pre-defined flags should be set.  At this point, we start
# using if statements to make use of that.

# Of course, there may not BE any flags.  If not, the script needs to know
# what to do.

#if ($argc == "1")
if ($arg->no_flags)
 { $arg->flag["use_message"] = "1"; }

# If something is entered that is not a valid flag, the system
# will append it to the bad_flags array.  The following statment
# deals with that.  A much simpler option is to simple print an error
# message and exit the script, but I like my error messages to be as
# informative as possible.  bad_flags[] is created with a value of null.

if ($arg->bad_flags)
{
 echo "The following flag";
 if (count($arg->bad_flags) > 1) { echo "s are "; }
 else { echo " is "; }
 echo "not valid:\n";
 for ($i = 0; $i < count($arg->bad_flags); $i++)
 {
  echo "-".$arg->bad_flags[$i]."\n";
 }
 $arg->flag["use_message"] = "1";
}

# This is where we begin parsing flags.  I prefer to use if statements,
# but you might prefer using switch statments, or some other method 
# entirely.  This is simply one way to do it, intended to illustrate how
# the object stores data.  Every index in the flag[] array is created 
# with a null value, and will therefore be undefined unless the flag 
# has been specified on the command line..


if ($arg->flag["use_message"])
{
 # Print out a basic help message.  This was triggered by the -h or 
 # --help flag.
 echo "This demo only has a few options.  They are:
	-h | --help   			displays this message.
	-c  				clears the screen -- note that there
					is no short form!
	-v value | --value value	sets a variable to value.
        -t value1 value2 		
        --twovars value1 value2 	inserts two values into a variable, 
					separated by commas.
	-f | --first			Displays a short message.
	-s | --second			Displays a short message.

\n";
 exit("0"); # Exit the script if this shows up.
}

if ($arg->flag["screen_clear"])
{
 # There's no particular value in having a way to clear the screen, but
 # whatever.
 echo "I will clear the screen in 5 seconds.\n";
 sleep(5);
 system('clear');
}

if (isset($arg->flag["value"]))
{
 # A basic use of a flag with an extra argument.
 echo "Value of \$arg->flag[\"value\"] has been set to '".$arg->flag["value"]."'\n";
} 

if ($arg->flag["multivar"])
{
 # Basic use of a flag with multiple arguments.
 echo "The value of multivar is currently '".$arg->flag["multivar"]."'\n";
}

if ($arg->flag["message"])
{
 # Just a basic flag.
 echo "\n\nThis is a message, since you used the --message flag.\n";
}

if ($arg->flag["first"])
{
  echo "You used the flag 'first'.\n";
}

if ($arg->flag["second"])
{
  echo "You used the flag 'second'.\n";
}
?>


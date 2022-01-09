# OpcLog
**Read Array from OPC server and paste sorted to excel sheet**

After user enter correct IP address for existing OPC UA Server -><br />
-> created Client is trying to establish connection -><br />
-> if successful, depends of user choice "SSI_TL_Logging_DB"."entry" or "SSI_TL_Logging_TC_DB"."entry" log array is read -><br />
-> then in %USERPROFILE% path (C:\Users\piotr in my case), .cvs file is created and finally opened  🙃

**todo:**<br />
- take into account also: <br /> 
  "SSI_GS_Call_IDB"."appender_i"."para"."log_level".value.set := 4; // Loglevel of logging events
  "CS_GS_Call_IDB"."para"."lu_data_log_level".value.act := 4; // Loglevel of LU data-logging events
  "SSI_GS_Call_IDB"."para"."lists_log_level".value.act := 4; // Loglevel of list-logging events
  "SSI_GS_Call_IDB"."para"."tc_log_level".value.act := 4; // Loglevel of TC-logging events

# OpcLog
**Read Array from OPC server and paste sorted to excel sheet**

After user enter correct IP address for existing OPC UA Server -><br />
-> created Client is trying to establish connection -><br />
-> if successful, depends of user choice "SSI_TL_Logging_DB"."entry" or "SSI_TL_Logging_TC_DB"."entry" log array is read -><br />
-> then in %USERPROFILE% path (C:\Users\piotr in my case), .cvs file is created and finally opened  🙃

**todo:**<br />
- change all nodes level log "SSI_HMI_CPM_DB".node[#IO_EL_INFO.node_id].log_level to FATAL(1) <br /> 
  then only selected by user leave for DEBUG(5)/TRACE(6)
- remember last entered IP

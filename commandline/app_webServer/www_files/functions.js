// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
var chart;
var dps = [];
var xVal = 0;
var yVal = 100; 
var deviceCount;
var connectionState;
var firstUpdate = true;
var updateInterval = 500; // in miliseconds
var dataLength = 500; // number of dataPoints visible at any point

/*---------------------------------------------------------------------------*/
function searchDevices()
{
  jQuery(function(){
    $.ajax(
    {
      url: 'http://localhost:8000/search',
      method: 'POST',
      dataType: 'json',        
      crossDomain: true,
      success: function(json) 
      {
        $('#myTextArea').append("> " + json.deviceCount.toString() + " device(s) found.\n");    
        $('#myTextArea').scrollTop($('#myTextArea')[0].scrollHeight);    
        firstUpdate = true;
        deviceCount = json.deviceCount;
      }
    });        
  });
}

/*---------------------------------------------------------------------------*/
function createTableHeaders()
{
  var table = document.getElementById("myTable");
  var row;
  var cell1;
  var cell2;
  var cell3;
  var currentElements = document.getElementById("myTable").rows.length;

  for(var i=0; i<currentElements;i++)
  {
    document.getElementById("myTable").deleteRow(0);
  }

  for(var i=0; i<deviceCount;i++)
  {
    row = table.insertRow(0);
    cell1 = row.insertCell(0);
    cell2 = row.insertCell(1);
    cell3 = row.insertCell(2);
    cell1.innerHTML = "";
    cell2.innerHTML = "";
    cell3.innerHTML = "";  
  }  

  row = table.insertRow(0);
  cell1 = row.insertCell(0);
  cell2 = row.insertCell(1);
  cell3 = row.insertCell(2);
  cell1.innerHTML = "ID";
  cell2.innerHTML = "Thermocouple";
  cell3.innerHTML = "Cold Junction";
}

/*---------------------------------------------------------------------------*/
function checkServerConnection()
{
  connectionState = true;
  return connectionState;
}

/*---------------------------------------------------------------------------*/
window.onload = function () 
{  
  $('#myTextArea').append("> Log started.\n");    
  $('#myTextArea').scrollTop($('#myTextArea')[0].scrollHeight); 

  /* Always returns true ... */
  if(checkServerConnection() == true)
  {
    $('#myTextArea').append("> Connection OK!\n");    
    $('#myTextArea').scrollTop($('#myTextArea')[0].scrollHeight);     
  }
  else
  {
    $('#myTextArea').append("> Connection problem ...\n");    
    $('#myTextArea').scrollTop($('#myTextArea')[0].scrollHeight);     
  }

  chart = new CanvasJS.Chart("chartContainer",{
    axisX:{      
      valueFormatString: "DD/MM HH:mm:ss:ff" ,
    },
    data: [{
      type: "line",
      dataPoints: dps 
    }]
  });  
  chart.render();   

  searchDevices();

  var updateChart = function() 
  {
    jQuery(function(){
      $.ajax(
      {
        url: 'http://localhost:8000/read',
        method: 'POST',
        dataType: 'json',        
        crossDomain: true,
        success: function(json) 
        {          
          if(firstUpdate == true)
          {
            firstUpdate = false;
            deviceCount = json.deviceCount;
            createTableHeaders();
            while(dps.length)
            {
              dps.pop();
            }
          }
          else
          {        
            var table = document.getElementById("myTable");          
            for(var i=0; i<deviceCount; i++)
            {
              var row = table.rows[i+1];
              var cell1 = row.cells[0];
              var cell2 = row.cells[1];
              var cell3 = row.cells[2];
              cell1.innerHTML = json.readings[i].ID.toString();
              cell2.innerHTML = json.readings[i].TC.toString();
              cell3.innerHTML = json.readings[i].CJ.toString();
            }

            dps.push({
              x: new Date(Date.now()),
              y: json.readings[0].TC
            });
          
            if (dps.length > dataLength)
            {
              dps.shift();        
            }
          }
          
          chart.render();   
        }
      });        
    });
  };

  if(connectionState == true)
  {
    // update chart after specified time. 
    setInterval(function(){updateChart()}, updateInterval); 
  }

}
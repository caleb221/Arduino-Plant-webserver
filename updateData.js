


var xmlHttp=createXmlHttpObject();
    
function createXmlHttpObject(){ 
    
    if(window.XMLHttpRequest){ 
        xmlHttp=new XMLHttpRequest();
        }
    else
    { 
        xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');   
    } 
    return xmlHttp;
    } 
     
    function response(){ 
        
    var t,h,w,l,xmlResponse;
    var tempData,humidData,waterData,lightData;
        
    xmlResponse=xmlHttp.responseXML;
    
    t = xmlResponse.getElementsByTagName('temp');
    h =xmlResponse.getElementsByTagName('humid');
    w=xmlResponse.getElementsByTagName('water');
    l=xmlResponse.getElementsByTagName('light');
    
    tempData = t[0].firstChild.nodeValue; 
    humidData = h[0].firstChild.nodeValue; 
    waterData = w[0].firstChild.nodeValue; 
    lightData = l[0].firstChild.nodeValue; 
    
    document.getElementById('temp').innerHTML=tempData;
    document.getElementById('humid').innerHTML=humidData;
    document.getElementById('water').innerHTML=waterData;
    document.getElementById('light').innerHTML=lightData;
        
    } 

    //CLIENT UPDATE
    function process(){ 
    xmlHttp.open('PUT','xml',true); 
    xmlHttp.onreadystatechange=response; 
    xmlHttp.send(null); 
    setTimeout('process()',200); 
    }

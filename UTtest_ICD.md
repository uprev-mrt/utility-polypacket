# test ICD
* Generated: 06/06/19<br/>
* CRC: 706C9397
* Transport Encoding: (COBS) [Consistent Overhead ByteStuffing](https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing)

** This is a sample protocol used for unit testing the Poly Packet module **

---
<div id="header" class="packet">
<h2>Header </h2>
<hr/>

<p class="desc">All packets contain a standard header</p>
<br/>
<b>Structure:</b>
<table class="fixed" style="width:40%">
  <tr>
  <th  >Byte</th>
        <th >0</th>
        <th >1</th>
        <th >2</th>
        <th >3</th>
        <th >4</th>
        <th >5</th>
        <th >6</th>
  </tr>
  <tr>
    <td>Field</td>
      <td colspan="1">Id</td>
      <td colspan="2">Length</td>
      <td colspan="2">Token</td>
      <td colspan="2">Checksum</td>
  </tr>
  <tr>
    <td>Type</td>
    <td colspan="1">uint8_t</td>
    <td colspan="2">uint16_t</td>
    <td colspan="2">uint16_t</td>
    <td colspan="2">uint16_t</td>
  </tr>
</table>
<br/>
<b>Fields:</b>
<table class="fields">
  <tr>
    <th> Field</th>
    <th> Description</th>
  </tr>
  <tr>
    <td width="">Id</td>
    <td>Packet Type identifier</td>
  </tr>
  <tr>
    <td width="">Length</td>
    <td>Number of bytes in packet (not including header)</td>
  </tr>
  <tr>
    <td width="">Token</td>
    <td>Psuedo random token generated for message</td>
  </tr>
  <tr>
    <td width="">Checksum</td>
    <td>16bit checksum used for data validation</td>
  </tr>
</table>

<br/>
<hr class="thick">

</div>

<h2> Packet Types </h2>
<hr/>

<ul>
  <li><a href="#packet_ping">[00]  Ping </a></li>
  <li><a href="#packet_ack">[01]  Ack </a></li>
  <li><a href="#packet_sendcmd">[02]  SendCmd </a></li>
  <li><a href="#packet_getdata">[03]  GetData </a></li>
  <li><a href="#packet_data">[04]  Data </a></li>
</ul>

<hr class="thick">

<div class="packet" id="packet_ping">
<h2>Ping </h2>
<hr/>
<ul>
  <li class="note">Packet ID: <b>[00]</b></li>
  <li class="note"> Requests: <a href="#packet_ack">Ack</a></li>
</ul>

<span class="note"> This Packet type does not contain any data fields </span><br/>
<br/>
<hr class="thick">
</div>

<div class="packet" id="packet_ack">
<h2>Ack </h2>
<hr>
<ul>
  <li class="note">  Packet ID: <b>[01]</b></li>
  <li class="note">Responds To: <a href="#packet_ping">Ping</a></li>
</ul>

<span class="note"> This Packet type does not contain any data fields </span><br/>
<br/>
<hr class="thick">
</div>


<div id="packet_sendcmd" class="packet">
<h2>SendCmd </h2>
<hr/>
<ul>
  <li class="note">  Packet ID: <b>[02]</b></li>
</ul>

<p class="desc">Message to set command in node</p>
<br/>
<b>Structure:</b>
<table class="fixed" >
  <tr>
  <th  >Byte</th>
      <th >0</th>
      </tr>
  <tr>
    <td>Field</td>
      <td colspan="1">cmd</td>
  </tr>
  <tr>
    <td>Type</td>
      <td colspan="1">    uint8_t  </td>
  </tr>
</table>
<br/>
<b>Fields:</b>
<table class="fields">
  <tr>
    <th> Field</th>
    <th> Description</th>
  </tr>
  <tr>
    <td width="">cmd</td>
    <td>      <br/>
      <ul>
      <li class="val">0x00 : <b>LED_ON</b> - turns on led</li>
      <li class="val">0x01 : <b>LED_OFF</b> - turns off led</li>
      <li class="val">0x02 : <b>RESET</b> - resets the device</li>
      </ul>
    </td>
  </tr>
</table>

<br/>
<hr class="thick">
</div>
<div id="packet_getdata" class="packet">
<h2>GetData </h2>
<hr/>
<ul>
  <li class="note">  Packet ID: <b>[03]</b></li>
  <li class="note">   Requests: <a href="#packet_data">Data</a></li>
</ul>

<p class="desc">Message to get data from node</p>
<br/>
<span class="note"> This Packet type does not contain any data fields </span><br/>
<br/>
<hr class="thick">
</div>
<div id="packet_data" class="packet">
<h2>Data </h2>
<hr/>
<ul>
  <li class="note">  Packet ID: <b>[04]</b></li>
  <li class="note">Responds To:   <a href="#packet_getdata">GetData</a>  </li>
</ul>

<p class="desc">Message containing data from sensor</p>
<br/>
<b>Structure:</b>
<table class="fixed" >
  <tr>
  <th  >Byte</th>
      <th >0</th>
        <th >1</th>
        <th >2</th>
        <th >3</th>
        <th >4</th>
        <th >5</th>
        <th >6</th>
    <th colspan="2">........</th>
    <th >38</th>
  </tr>
  <tr>
    <td>Field</td>
      <td colspan="2">sensorA</td>
      <td colspan="4">sensorB</td>
      <td colspan="4">sensorName</td>
  </tr>
  <tr>
    <td>Type</td>
      <td colspan="2">    int16_t  </td>
      <td colspan="4">    int  </td>
      <td colspan="4">    char    [32]  </td>
  </tr>
</table>
<br/>
<b>Fields:</b>
<table class="fields">
  <tr>
    <th> Field</th>
    <th> Description</th>
  </tr>
  <tr>
    <td width="">sensorA</td>
    <td>Value of Sensor A    </td>
  </tr>
  <tr>
    <td width="">sensorB</td>
    <td>Value of Sensor B    </td>
  </tr>
  <tr>
    <td width="">sensorName</td>
    <td>Name of sensor responding to message     </td>
  </tr>
</table>

<br/>
<hr class="thick">
</div>
</div>
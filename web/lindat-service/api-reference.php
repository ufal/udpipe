<?php $main_page=basename(__FILE__); require('header.php') ?>

<div class="dropdown pull-right" style='margin-left: 10px; margin-bottom: 10px'>
  <button class="btn btn-default dropdown-toggle" type="button" id="tocDropdown" data-toggle="dropdown"><span class="fa fa-bars"></span> Table of Contents <span class="caret"></span></button>
  <ul class="dropdown-menu dropdown-menu-right" aria-labelledby="tocDropdown">
    <li><a href="#api_reference">API Reference</a></li>
    <li><a href="#models"><span class="fa fa-caret-right"></span> <code>models</code></a></li>
    <li><a href="#process"><span class="fa fa-caret-right"></span> <code>process</code></a></li>
    <li class="divider"></li>
    <li><a href="#model_selection">Model Selection</a></li>
    <li class="divider"></li>
    <li><a href="#using_curl">Accessing API using Curl</a></li>
  </ul>
</div>

<p>UDPipe web service is available on
<code>http(s)://lindat.mff.cuni.cz/services/udpipe/api/</code>.</p>

<p>The web service is freely available. Respect the
<a href="http://creativecommons.org/licenses/by-nc-sa/4.0/">CC BY-NC-SA</a>
licence of the models &ndash; <b>explicit written permission of the authors is
required for any commercial exploitation of the system</b>. If you use the
service, you agree that data obtained by us during such use can be used for further
improvements of the systems at UFAL. All comments and reactions are welcome.</p>

<h2 id="api_reference">API Reference</h2>

<p>The UDPipe REST API can be accessed directly or via any other web
programming tools that support standard HTTP request methods and JSON for output
handling.</p>

<table class='table table-striped table-bordered'>
<tr>
    <th>Service Request</th>
    <th>Description</th>
    <th>HTTP Method</th>
</tr>
<tr>
    <td><a href="#models">models</a></td>
    <td>return list of models and supported methods</td>
    <td>GET/POST</td>
</tr>
<tr>
    <td><a href="#process">process</a></td>
    <td><a href="http://ufal.mff.cuni.cz/udpipe/users-manual#run_udpipe">process supplied data</a></td>
    <td>GET/POST</td>
</tr>
</table>

<h3>Method <a id='models'>models</a></h3>

<p>Return the list of models available in the UDPipe REST API, and for each
model enumerate components supported by this models (model can contain
a <code>tokenizer</code>, <code>tagger</code> and a <code>parser</code>).
The default model (used when user supplies no model to a method call) is
also returned &ndash; this is guaranteed to be the latest Czech model.</p>

<h4>Browser Example</h4>

<table style='width: 100%'>
 <tr><td style='vertical-align: middle'><pre style='margin-bottom: 0; white-space: pre-wrap' class="prettyprint lang-html">http://lindat.mff.cuni.cz/services/udpipe/api/models</pre></td>
     <td style='vertical-align: middle; width: 6em'><button style='width: 100%' type="button" class="btn btn-success btn-xs" onclick="window.open('http://lindat.mff.cuni.cz/services/udpipe/api/models')">try&nbsp;this</button></td></tr>
</table>

<h4>Example JSON Response</h4>
<pre class="prettyprint lang-json">
{
 "models": {
  "czech-ud-1.2-160523": ["tokenizer", "tagger", "parser"],
  "english-ud-1.2-160523": ["tokenizer", "tagger", "parser"]
 },
 "default_model": "czech-ud-1.2-160523"
}
</pre>

<hr />

<h3>Method <a id='process'>process</a></h3>

<p>Process given data as described <a href="http://ufal.mff.cuni.cz/udpipe/users-manual#run_udpipe">in the User's Manual</a>.</p>

<table class='table table-striped table-bordered'>
<tr><th>Parameter</th><th>Mandatory</th><th>Data type</th><th>Description</th></tr>
<tr><td>data</td><td>yes</td><td>string</td><td>Input text in <b>UTF-8</b>.</td></tr>
<tr><td>model</td><td>no</td><td>string</td><td>Model to use; see <a href="#model_selection">model selection</a> for model matching rules.</td></tr>
<tr><td>tokenizer</td><td>no</td><td>string</td><td>If the option is present, the input is assumed to be in plain text and is tokenized. If the parameter has a value, it is passed to the tokenizer.</td></tr>
<tr><td>input</td><td>no</td><td>string (<code>conllu</code> / <code>horizontal</code> / <code>vertical</code>)</td><td>If the tokenizer is not used, the input is assumed to be in the specified <a href="http://ufal.mff.cuni.cz/udpipe/users-manual#run_udpipe_input">input format</a>; default <code>conllu</code>.</td></tr>
<tr><td>tagger</td><td>no</td><td>string</td><td>If the option is present, the input is POS tagged and lemmatized. If the parameter has a value, it is passed to the tagger.</td></tr>
<tr><td>parser</td><td>no</td><td>string</td><td>If the option is present, the input is dependency parsed. If the parameter has a value, it is passed to the parser.</td></tr>
<tr><td>output</td><td>no</td><td>string (<code>conllu</code> / <code>horizontal</code> / <code>vertical</code>)</td><td>The <a href="http://ufal.mff.cuni.cz/udpipe/users-manual#run_udpipe_output">output format</a> to use; default <code>conllu</code>.</td></tr>
</table>

<p>
The response is in <a href="http://en.wikipedia.org/wiki/JSON">JSON</a> format of the
following structure:</p>

<pre class="prettyprint lang-json">
{
 "model": "Model used",
 "acknowledgements": ["URL with acknowledgements", ...],
 "result": "processed_output"
}
</pre>

The <code>processed_output</code> is the output of the UDPipe in the requested output format.

<h4>Browser Examples</h4>
<table style='width: 100%'>
 <tr><td style='vertical-align: middle'><pre style='margin-bottom: 0; white-space: pre-wrap' class="prettyprint lang-html">http://lindat.mff.cuni.cz/services/udpipe/api/process?tokenizer&amp;tagger&amp;parser&amp;data=Děti pojedou k babičce. Už se těší.</pre></td>
     <td style='vertical-align: middle; width: 6em'><button style='width: 100%' type="button" class="btn btn-success btn-xs" onclick="window.open('http://lindat.mff.cuni.cz/services/udpipe/api/process?tokenizer&amp;tagger&amp;parser&amp;data=Děti pojedou k babičce. Už se těší.')">try&nbsp;this</button></td></tr>
</table>

<hr />

<h2 id="model_selection">Model Selection</h2>

<p>There are several possibilities how to select required model using
the <code>model</code> option:</p>

<ul>
  <li>If <code>model</code> option is not specified, the default model
  (returned by <a href="#models">models</a> method) is used &ndash; this is
  guaranteed to be the latest Czech model.</li>

  <li>The <code>model</code> option can specify one of the models returned
  by the <a href="#models">models</a> method.</li>

  <li>Version info in the <code>-YYMMDD</code> format can be left out when
  supplying <code>model</code> option &ndash; the latest avilable model will be
  used.</li>

  <li>The <code>model</code> option may be only several first words of model
  name. In this case, the latest most suitable model is used.</li>
</ul>

<p><span class="fa fa-info-circle"></span> Note that the last possibility allows using <code>czech</code> or <code>english</code> as models.</p>

<hr />

<h2 id="using_curl">Accessing API using Curl</h2>

The described API can be comfortably used by <code>curl</code>. Several examples follow:

<h3>Passing Input on Command Line (if UTF-8 locale is being used)</h3>
<pre style="white-space: pre-wrap" class="prettyprint lang-sh">curl --data 'tokenizer=&tagger=&parser=&data=Děti pojedou k babičce. Už se těší.' http://lindat.mff.cuni.cz/services/udpipe/api/process</pre>

<h3>Using Files as Input (files must be in UTF-8 encoding)</h3>
<pre style="white-space: pre-wrap" class="prettyprint lang-sh">curl -F 'data=@input_file' http://lindat.mff.cuni.cz/services/udpipe/api/process</pre>

<h3>Specifying Additional Parameters</h3>
<pre style="white-space: pre-wrap" class="prettyprint lang-sh">curl -F 'data=@input_file' -F 'model=english' http://lindat.mff.cuni.cz/services/udpipe/api/process</pre>

<h3>Converting JSON Result to Plain Text</h3>
<pre style="white-space: pre-wrap" class="prettyprint lang-sh">curl -F 'data=@input_file' http://lindat.mff.cuni.cz/services/udpipe/api/process | python -c "import sys,json; sys.stdout.write(json.load(sys.stdin)['result'])"</pre>

<?php require('footer.php') ?>

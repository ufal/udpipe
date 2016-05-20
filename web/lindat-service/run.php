<?php $main_page=basename(__FILE__); require('header.php') ?>

<?php require('about.html') ?>

<p>
Description of the available methods is available in the <a href="api-reference.php">API
Documentation</a> and the models are described in the
<a href="http://ufal.mff.cuni.cz/udpipe/users-manual">UDPipe User's Manual</a>.
</p>

<p><b>The service will be released soon.</b></p>
<!--
<script type="text/javascript"><!- -
  var input_file_content = null;
  var output_file_content = null;
  var output_file_tree = null;

  function doSubmit() {
    var model = jQuery('#model :selected').text();
    if (!model) return;

    var text;
    var input_tab = jQuery('#input_tabs>.tab-pane.active');
    if (input_tab.length > 0 && input_tab.attr('id') == 'input_file') {
      if (!input_file_content) { alert('Please load file first.'); return; }
      text = input_file_content;
    } else {
      text = jQuery('#input').val();
    }

    output_file_content = null;
    output_file_tree = null;
    jQuery('#submit').html('<span class="fa fa-cog"></span> Waiting for Results <span class="fa fa-cog"></span>');
    jQuery('#submit').prop('disabled', true);
    var options = {model: model, data: text};
    jQuery.ajax('//lindat.mff.cuni.cz/services/parsito/api/parse',
           {dataType: "json", data: options, type: "POST", success: function(json) {
      try {
        if ("result" in json) {
          output_file_content = json.result;
          jQuery('#output_text').html('<button id="save_file" class="btn btn-success form-control" type="submit" onclick="saveFile()"><span class="fa fa-download"></span> Save Output File</span></button><div class="well" id="output_text_content" style="white-space: pre-wrap; margin-top: 15px"></div>');
          jQuery('#output_text_content').text(output_file_content);

          var output_tab = jQuery('#output_tabs>.tab-pane.active');
          if (output_tab.length > 0 && output_tab.attr('id') == 'output_tree') showTree();
          else jQuery('#output_tree').empty();

          var acknowledgements = "";
          for (var a in json.acknowledgements)
            acknowledgements += "<a href='" + json.acknowledgements[a] + "'>" + json.acknowledgements[a] + "</a><br/>";
          jQuery('#acknowledgements_text').html(acknowledgements).show();
          jQuery('#acknowledgements_title').show();
          jQuery('#acknowledgements_text').show();
        }
      } catch(e) {
        jQuery('#submit').html('<span class="fa fa-arrow-down"></span> Process Input <span class="fa fa-arrow-down"></span>');
        jQuery('#submit').prop('disabled', false);
      }
    }, error: function(jqXHR, textStatus) {
      jQuery('#output_text').empty();
      jQuery('#output_tree').empty();
      alert("An error occurred" + ("responseText" in jqXHR ? ": " + jqXHR.responseText : "!"));
    }, complete: function() {
      jQuery('#submit').html('<span class="fa fa-arrow-down"></span> Process Input <span class="fa fa-arrow-down"></span>');
      jQuery('#submit').prop('disabled', false);
    }});
  }

  function saveFile() {
    if (!output_file_content) return;
    var conllu = new Blob([output_file_content], {type: "text/x-conllu"});
    saveAs(conllu, "parsed.conllu");
  }

  function showTree() {
    if (!output_file_content) return;
    if (output_file_tree) return;

    var trees = [];
    var tree_desc = [];
    var tree_nodes = [];

    var lines = output_file_content.split(/\r?\n/);
    lines.push('');
    for (var i in lines) {
      if (lines[i].match(/^(#|\d+-)/)) continue;
      if (lines[i]) {
        var parts = lines[i].split('\t');
        for (var i in parts) if (parts[i] == "_") parts[i] = "";

        if (tree_desc.length) tree_desc.push([' ', 'space']);
        tree_desc.push([parts[1], 'w'+parts[0]]);

        if (!tree_nodes.length) tree_nodes.push({id:'w0', ord:0, parent:null, data:{id:"0",form:"<root>"}, labels:['<root>','','']});
        tree_nodes.push({id:'w'+parts[0], ord:tree_nodes.length, parent:'w'+parts[6], data:{
          id:parts[0], form:parts[1], lemma:parts[2], upostag:parts[3], xpostag:parts[4],
          feats:parts[5], head:parts[6], deprel:parts[7], deps:parts[8], misc:parts[9]
        }, labels:[parts[1], '#{#00008b}'+parts[7], '#{#004048}'+parts[3]]});
      } else if (tree_nodes.length) {
        var last_child = [];
        for (var i = 1; i < tree_nodes.length; i++) {
          var head = parseInt(tree_nodes[i].data.head);
          if (!last_child[head]) tree_nodes[head].firstson = 'w'+i;
          else tree_nodes[last_child[head]].rbrother = 'w'+i;
          last_child[head] = i;
        }

        trees.push({desc:tree_desc,zones:{conllu:{trees:{"a":{layer:"a",nodes:tree_nodes}}}}});
        tree_desc = [];
        tree_nodes = [];
      }
    }

    output_file_tree = trees;
    jQuery('#output_tree').html('<button id="save_tree" class="btn btn-success form-control" type="submit" onclick="saveTree()"><span class="fa fa-download"></span> Save Tree as SVG</span></button><div id="output_tree_content" style="margin-top: 15px"></div>');
    setTimeout(function() { jQuery('#output_tree_content').treexView(trees); }, 0);
  }

  function saveTree() {
    var svg_element = jQuery('#output_tree_content svg');
    if (svg_element.length) {
      var svg = new Blob([svg_element.parent().html()], {type: "image/svg+xml"});
      saveAs(svg, 'parsed.svg');
    }
  }

  jQuery(document).on('change', '#input_file_field', function() {
    jQuery('#input_file_name').text();
    input_file_content = null;
    if (this.files.length > 0) {
      var file = this.files[0];
      jQuery('#input_file_name').text(file.name + ' (loading...)');
      if (!window.FileReader) {
        jQuery('#input_file_name').text(file.name + ' (load error - file loading API not supported, please use newer browser)').wrapInner('<span class="text-danger"></span>');
      } else {
        var reader = new FileReader();
        reader.onload = function() {
          input_file_content = reader.result;
          jQuery('#input_file_name').text(file.name + ' (' + (input_file_content.length/1024).toFixed(1) + 'kb loaded)');
        }
        reader.onerror = function() {
          jQuery('#input_file_name').text(file.name + ' (load error)').wrapInner('<span class="text-danger"></span>');
        }
        reader.readAsText(file);
      }
    }
  });

  jQuery(document).on('show.bs.tab', '#output_tree_link', showTree);

  jQuery(document).ready(function() {
    jQuery.ajax('//lindat.mff.cuni.cz/services/parsito/api/models',
                {dataType: "json", success: function(json) {
      var models_list = '';
      for (var model in json.models)
          models_list += "<option" + (models_list ? "" : " selected") + ">" + json.models[model] + "</option>";
      jQuery('#model').html(models_list);
    }, complete: function() {
      if (!jQuery('#model').html()) {
        jQuery('#error').text("Cannot obtain the list of models from the service.").show();
      }
    }});
  });
- -></script>

<div class="panel panel-info">
  <div class="panel-heading">Service</div>
  <div class="panel-body">

    <p>The service is freely available for testing. Respect the
    <a href="http://creativecommons.org/licenses/by-nc-sa/4.0/">CC BY-NC-SA</a>
    licence of the models &ndash; <b>explicit written permission of the authors is
    required for any commercial exploitation of the system</b>. If you use the
    service, you agree that data obtained by us during such use can be used for further
    improvements of the systems at UFAL. All comments and reactions are welcome.</p>

    <div id="error" class="alert alert-danger" style="display: none"></div>
    <div class="form-horizontal">
      <div class="form-group row">
        <label class="col-sm-2 control-label">Model:</label>
        <div class="col-sm-10">
          <select id="model" class="form-control"></select>
        </div>
      </div>
      <div class="form-group row">
        <label class="col-sm-2 control-label">Input:</label>
        <div class="col-sm-10">
          <label class="radio-inline" id="option_input_conllu"><input name="option_input" type="radio" value="conllu" checked/>CoNLL-U</label>
        </div>
      </div>
      <div class="form-group row">
        <label class="col-sm-2 control-label">Output:</label>
        <div class="col-sm-10">
          <label class="radio-inline" id="option_output_conllu"><input name="option_output" type="radio" value="conllu" checked/>CoNLL-U</label>
        </div>
      </div>
    </div>

    <ul class="nav nav-tabs nav-justified nav-tabs-green">
     <li class="active"><a href="#input_text" data-toggle="tab"><span class="fa fa-font"></span> Input Text</a></li>
     <li><a href="#input_file" data-toggle="tab"><span class="fa fa-file-text-o"></span> Input File</a></li>
    </ul>
    <div class="tab-content" id="input_tabs" style="border-right: 1px solid #ddd; border-left: 1px solid #ddd; border-bottom: 1px solid #ddd; border-bottom-right-radius: 5px; border-bottom-left-radius: 5px; padding: 15px">
     <div class="tab-pane active" id="input_text">
      <textarea id="input" class="form-control" rows="10" cols="80">1	Z	_	ADP	_	AdpType=Prep|Case=Gen	_	_	_	_
2	očí	_	NOUN	_	Case=Gen|Gender=Fem|Negative=Pos|Number=Plur	_	_	_	_
3	do	_	ADP	_	AdpType=Prep|Case=Gen	_	_	_	_
4	očí	_	NOUN	_	Case=Gen|Gender=Fem|Negative=Pos|Number=Plur	_	_	_	_</textarea>
     </div>
     <div class="tab-pane" id="input_file">
      <div class="input-group">
       <div class="form-control" id="input_file_name"></div>
       <span class="input-group-btn"><span class="btn btn-success btn-file">Load File ... <input type="file" id="input_file_field"></span></span>
      </div>
     </div>
    </div>

    <button id="submit" class="btn btn-primary form-control" type="submit" style="margin-top: 15px; margin-bottom: 15px" onclick="doSubmit()"><span class="fa fa-arrow-down"></span> Process Input <span class="fa fa-arrow-down"></span></button>

    <ul class="nav nav-tabs nav-justified nav-tabs-green">
     <li class="active"><a href="#output_text" data-toggle="tab"><span class="fa fa-font"></span> Output Text</a></li>
     <li><a href="#output_tree" data-toggle="tab" id="output_tree_link"><span class="fa fa-tree"></span> Show Trees</a></li>
    </ul>
    <div class="tab-content" id="output_tabs" style="border-right: 1px solid #ddd; border-left: 1px solid #ddd; border-bottom: 1px solid #ddd; border-bottom-right-radius: 5px; border-bottom-left-radius: 5px; padding: 15px">
     <div class="tab-pane active" id="output_text">
     </div>
     <div class="tab-pane" id="output_tree">
     </div>
    </div>

    <h3 id="acknowledgements_title" style="display: none; margin-top: 15px">Acknowledgements</h3>
    <p id="acknowledgements_text" style="display: none"> </p>
  </div>
</div>
-->

<?php require('footer.php') ?>

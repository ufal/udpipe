<?php $main_page=basename(__FILE__); require('header.php') ?>

<?php require('about.html') ?>

<p>
Description of the available methods is available in the <a href="api-reference.php">API
Documentation</a> and the models are described in the
<a href="http://ufal.mff.cuni.cz/udpipe/users-manual">UDPipe User's Manual</a>.
</p>

<script type="text/javascript"><!--
  var models = {};
  var input_file_content = null;
  var output_file_content = null;
  var output_file_table = null;
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

    var options = {model: model, data: text};
    var input = jQuery('input[name=option_input]:checked').val();
    if (input && input == "tokenizer") {
      options.tokenizer = "";
      var opts = ["presegmented", "ranges", "normalized_spaces"];
      for (var i in opts)
        if (jQuery("#tokenizer_" + opts[i]).prop('checked')) options.tokenizer += (options.tokenizer ? ";" : "") + opts[i];
    } else {
      options.input = input ? input : "conllu";
    }
    if (jQuery('#tagger').prop('checked')) options.tagger = "";
    if (jQuery('#parser').prop('checked')) options.parser = "";

    var form_data = null;
    if (window.FormData) {
      form_data = new FormData();
      for (var key in options)
        form_data.append(key, options[key]);
    }

    output_file_content = null;
    output_file_table = null;
    output_file_tree = null;
    jQuery('#submit').html('<span class="fa fa-cog"></span> Waiting for Results <span class="fa fa-cog"></span>');
    jQuery('#submit').prop('disabled', true);
    jQuery.ajax('//lindat.mff.cuni.cz/services/udpipe/api/process',
           {data: form_data ? form_data : options, processData: form_data ? false : true,
            contentType: form_data ? false : 'application/x-www-form-urlencoded; charset=UTF-8',
            dataType: "json", type: "POST", success: function(json) {
      try {
        if ("result" in json) {
          output_file_content = json.result;
          jQuery('#output_text').html('<button id="save_file" class="btn btn-success form-control" type="submit" onclick="saveFile()"><span class="fa fa-download"></span> Save Output File</span></button><div class="well" id="output_text_content" style="white-space: pre-wrap; margin-top: 15px"></div>');
          jQuery('#output_text_content').text(output_file_content);

          var output_tab = jQuery('#output_tabs>.tab-pane.active');
          if (output_tab.length > 0 && output_tab.attr('id') == 'output_table') showTable(); else jQuery('#output_table').empty();
          if (output_tab.length > 0 && output_tab.attr('id') == 'output_tree') showTree(); else jQuery('#output_tree').empty();

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
    saveAs(conllu, "processed.conllu");
  }

  function showTable() {
    if (!output_file_content) return;
    if (output_file_table) return;

    var table = [];
    table.push('<table class="table table-hover table-condensed" style="margin-top: 15px">');
    table.push('<tr><th>Id</th><th>Form</th><th>Lemma</th><th>UPosTag</th><th>XPosTag</th><th>Feats</th><th>Head</th><th>DepRel</th><th>Deps</th><th>Misc</th></tr>');
    var lines = jQuery('#output_text_content').html().split(/\r?\n/);
    while (lines.length && lines[lines.length-1] == "") lines.pop();
    for (var i in lines) {
      table.push(lines[i].match(/^(#|$)/) ?
        '<tr><td colspan="10">' + (lines[i] ? lines[i] : "&nbsp;") + '</td></tr>' :
        '<tr><td>' + lines[i].replace(/\t/g, '</td><td>').replace(/\|/g, "|<wbr>") + '</td></tr>'
      );
    }
    table.push('</table>');

    output_file_table = table.join("\n");
    jQuery('#output_table').html('<button id="save_file" class="btn btn-success form-control" type="submit" onclick="saveFile()"><span class="fa fa-download"></span> Save Output File</span></button>' + output_file_table);
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
        tree_nodes.push({id:'w'+parts[0], ord:tree_nodes.length, parent:parts[6]!==""?'w'+parts[6]:null, data:{
          id:parts[0], form:parts[1], lemma:parts[2], upostag:parts[3], xpostag:parts[4],
          feats:parts[5], head:parts[6], deprel:parts[7], deps:parts[8], misc:parts[9]
        }, labels:[parts[1], '#{#00008b}'+parts[7], '#{#004048}'+parts[3]]});
      } else if (tree_nodes.length) {
        var last_child = [];
        for (var i = 1; i < tree_nodes.length; i++) {
          var head = tree_nodes[i].data.head!=="" ? parseInt(tree_nodes[i].data.head) : "";
          if (head !== "") {
            if (!last_child[head]) tree_nodes[head].firstson = 'w'+i;
            else tree_nodes[last_child[head]].rbrother = 'w'+i;
            last_child[head] = i;
          }
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
      saveAs(svg, 'processed.svg');
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

  jQuery(document).on('show.bs.tab', '#output_table_link', showTable);
  jQuery(document).on('show.bs.tab', '#output_tree_link', showTree);

  function inputChanged() {
    var input = jQuery('input[name=option_input]:checked').val();
    if (input == "tokenizer")
      jQuery('#tokenizer_options').show();
    else
      jQuery('#tokenizer_options').hide();
  }

  function updateModels() {
    var family = jQuery('input[name=family]:checked').val();
    var suffix_match = new RegExp(family + ".*$");

    var czech_model = '';
    for (var model in models)
      if (model.indexOf(family) != -1)
        if (model.startsWith("czech")) {
          czech_model = model;
          break;
        }

    var models_list = '';
    var current_language = '';
    for (var model in models) {
      var family_index = model.indexOf(family);
      if (family_index == -1) continue;
      var treebank = model.substr(0, family_index);

      var new_language = false;
      if (!current_language || !treebank.startsWith(current_language)) {
        new_language = true;
        current_language = treebank.replace(/-.*/, "");
      }

      models_list += "<option data-content='<span style=\"display: inline-block; width: " + (new_language ? "2.5" : "3.5") + "em\"><img src=\"flags/" + treebank + ".png\" style=\"height: 1em\"></span>" + model + "'" + ((czech_model ? model == czech_model : !models_list) ? "selected" : "") + ">" + model + "</option>";
    }
    jQuery('#model').html(models_list);
    jQuery('#model').selectpicker('refresh');
  }

  jQuery(document).ready(function() {
    jQuery.ajax('//lindat.mff.cuni.cz/services/udpipe/api/models',
                {dataType: "json", success: function(json) {
      models = json.models;
      updateModels();
    }, complete: function() {
      if (jQuery('#model').html()) {
        fillUsingParams(function() { if (jQuery('#input').val()) doSubmit(); },
          [{param: 'model', selector: '#model', process: function(processed, value) {
             if (value in models) {
               processed(value);
             } else {
               jQuery.ajax('//lindat.mff.cuni.cz/services/udpipe/api/process',
                      {dataType: "json", data: {model: value, data: ''}, type: "POST", success: function(json) {
                        if ('model' in json) processed(json.model);
                      }});
             }}},
           {param: 'data', selector: '#input', process: function(processed, value) {
             var url_pattern = new RegExp(
               '^((news|(ht|f)tp(s?)):\\/\\/)((([a-z\\d]([a-z\\d-]*[a-z\\d])*)\\.)+[a-z]{2,}|'+
               '((\\d{1,3}\\.){3}\\d{1,3}))(\\:\\d+)?(\\/[-a-z\\d%_.~+]*)*'+
               '(\\?[;&a-z\\d%_.~+=-]*)?(\\#[-a-z\\d_]*)?$','i');
             if (url_pattern.test(value)) {
               jQuery.ajax(value, {dataType: 'text', success: function(result) { processed(result); }});
             } else {
               processed(value);
             }}}]);
      } else {
        jQuery('#error').text("Cannot obtain the list of models from the service.").show();
      }
    }});
  });
--></script>

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
          <label class="radio-inline"><input name="family" type="radio" value="-ud-2.3-" onchange="updateModels()" checked />UD 2.3 (<a href="http://ufal.mff.cuni.cz/udpipe/models#universal_dependencies_23_models">description</a>)</label>
          <label class="radio-inline"><input name="family" type="radio" value="-ud-2.0-1" onchange="updateModels()" />UD 2.0 (<a href="http://ufal.mff.cuni.cz/udpipe/models#universal_dependencies_20_models">description</a>)</label>
          <label class="radio-inline"><input name="family" type="radio" value="-ud-1.2-" onchange="updateModels()" />UD 1.2 (<a href="http://ufal.mff.cuni.cz/udpipe/models#universal_dependencies_12_models">description</a>)</label>
        </div>
      </div>
      <div class="form-group row">
        <div class="col-sm-offset-2 col-sm-10">
          <select id="model" class="selectpicker form-control" data-live-search="true"></select>
        </div>
      </div>
      <div class="form-group row">
        <label class="col-sm-2 control-label">Actions:</label>
        <div class="col-sm-10">
          <label class="checkbox-inline" title="Perform POS tagging an lemmatization"><input type="checkbox" id="tagger" checked>Tag and Lemmatize</label>
          <label class="checkbox-inline" title="Perform dependency parsing"><input type="checkbox" id="parser" checked>Parse</label>
        </div>
      </div>
      <div class="panel panel-default">
        <div class="panel-heading" role="tab" id="advancedHeading">
          <div class="collapsed" role="button" data-toggle="collapse" href="#advancedContent" aria-expanded="false" aria-controls="advancedContent">
            <span class="glyphicon glyphicon-triangle-bottom" aria-hidden="true"></span> Advanced Options
          </div>
        </div>
        <div id="advancedContent" class="panel-collapse collapse" role="tabpanel" aria-labelledby="advancedHeading">
          <div class="form-group row">
            <label class="col-sm-2 control-label">Input <a href="//ufal.mff.cuni.cz/udpipe/users-manual#run_udpipe_input">[?]</a>:</label>
            <div class="col-sm-10">
              <label title="Tokenize input using a tokenizer" class="radio-inline" id="option_input_tokenizer"><input name="option_input" type="radio" value="tokenizer" onchange="inputChanged()" checked/>Tokenize plain text</label>
              <label title="The CoNLL-U format" class="radio-inline" id="option_input_conllu"><input name="option_input" type="radio" value="conllu" onchange="inputChanged()" />CoNLL-U</label>
              <label title="Sentence on a line, words separated by spaces" class="radio-inline" id="option_input_horizontal"><input name="option_input" type="radio" value="horizontal" onchange="inputChanged()" />Horizontal</label>
              <label title="Word on a line, empty line denoting end of sentence" class="radio-inline" id="option_input_vertical"><input name="option_input" type="radio" value="vertical" onchange="inputChanged()" />Vertical</label>
            </div>
          </div>

          <div class="form-group row" id="tokenizer_options">
            <label class="col-sm-2 control-label">Tokenizer <a href="//ufal.mff.cuni.cz/udpipe/users-manual#run_udpipe_tokenizer">[?]</a>:</label>
            <div class="col-sm-10">
              <label class="checkbox-inline" title="Normalize spaces in input file"><input type="checkbox" id="tokenizer_normalized_spaces">Normalize spaces</label>
              <label class="checkbox-inline" title="The input is already segmented"><input type="checkbox" id="tokenizer_presegmented">Presegmented input</label>
              <label class="checkbox-inline" title="Save token ranges"><input type="checkbox" id="tokenizer_ranges">Save token ranges</label>
            </div>
          </div>
        </div>
      </div>
    </div>

    <ul class="nav nav-tabs nav-justified nav-tabs-green">
     <li class="active"><a href="#input_text" data-toggle="tab"><span class="fa fa-font"></span> Input Text</a></li>
     <li><a href="#input_file" data-toggle="tab"><span class="fa fa-file-text-o"></span> Input File</a></li>
    </ul>
    <div class="tab-content" id="input_tabs" style="border-right: 1px solid #ddd; border-left: 1px solid #ddd; border-bottom: 1px solid #ddd; border-bottom-right-radius: 5px; border-bottom-left-radius: 5px; padding: 15px">
     <div class="tab-pane active" id="input_text">
      <textarea id="input" class="form-control" rows="10" cols="80"></textarea>
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
     <li><a href="#output_table" data-toggle="tab" id="output_table_link"><span class="fa fa-table"></span> Show Table</a></li>
     <li><a href="#output_tree" data-toggle="tab" id="output_tree_link"><span class="fa fa-tree"></span> Show Trees</a></li>
    </ul>
    <div class="tab-content" id="output_tabs" style="border-right: 1px solid #ddd; border-left: 1px solid #ddd; border-bottom: 1px solid #ddd; border-bottom-right-radius: 5px; border-bottom-left-radius: 5px; padding: 15px">
     <div class="tab-pane active" id="output_text">
     </div>
     <div class="tab-pane" id="output_table">
     </div>
     <div class="tab-pane" id="output_tree">
     </div>
    </div>

    <h3 id="acknowledgements_title" style="display: none; margin-top: 15px">Acknowledgements</h3>
    <p id="acknowledgements_text" style="display: none"> </p>
  </div>
</div>

<?php require('footer.php') ?>

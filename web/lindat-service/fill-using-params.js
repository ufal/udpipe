function parseUrlParams(location) {
  var param_dict = {};

  var params = location.search.replace(/^\?/, '').split('&');
  for (var i in params) {
    var param = params[i];

    var equation_index = param.indexOf('=');
    if (equation_index == -1) equation_index = param.length;

    // Skip params with empty name
    if (!equation_index) continue;

    var param_name = decodeURIComponent(param.substr(0, equation_index));
    var param_value = decodeURIComponent(param.substr(equation_index + 1));
    param_dict[param_name] = param_value;
  }

  return param_dict;
}

function fillUsingParams(completed, mappings) {
  var url_params = parseUrlParams(window.location);

  function setValue(mapping, value) {
    var control = jQuery(mapping.selector);
    if (control.is('input')) {
      if (control.type == 'text' || control.type == 'hidden') {
        control.val(value);
      } else if (control.type == 'radio') {
        control.checked = true;
      }
    } else if (control.is('textarea')) {
      control.val(value);
    } else if (control.is('select')) {
      control.selectpicker('val', value);
    }
  }

  function processMappings() {
    while (mappings.length) {
      var mapping = mappings.shift();
      if (!(mapping.param in url_params)) continue;

      var param_value = url_params[mapping.param];
      if (!param_value) continue;

      return mapping.process(function(processed_value) { setValue(mapping, processed_value); processMappings(); }, param_value);
    }
    completed();
  }

  processMappings();
}

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

function fillUsingParams(mapping, button) {
  var url_params = parseUrlParams(window.location);

  for (var control_selector in mapping) {
    var value_from_param = mapping[control_selector];
    if (!(value_from_param in url_params)) continue;

    var value = url_params[value_from_param];
    if (!value) continue;

    var control = jQuery(control_selector);
    if (control.is('input')) {
      if (control.type == 'text' || control.type == 'hidden') {
        control.val(value);
      } else if (control.type == 'radio') {
        control.checked = true;
      }
    } else if (control.is('textarea')) {
      control.val(value);
    } else if (control.is('select')) {
      var options = jQuery('option', control);
      for (var i in options)
        if(options[i].text.startsWith(value)) {
          control.selectpicker('val', options[i].text);
          break;
        }
    }
  }
}

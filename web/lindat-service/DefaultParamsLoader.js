function loadParams(mapping, button) {

	var url = parseURL(window.location.href);

	Object.keys(mapping).forEach(function(key) {

		var control = $(key);
		var mapWith = mapping[key];
		var value = "";
		if(mapWith in url.params) {
			value = url.params[mapWith];
		}

		if(control.is("input")) {
			if(control.type == 'text' || control.type == 'hidden'){ 
				control.val(value);
			} else
			if(control.type == 'radio') {
				control.checked = true;
			}
		} else
		if(control.is("textarea")) {
			control.val(value);
		} else 
		if(control.is("select")) {
			opts = $('option', control);
			for(i=0;i<opts.length;i++) {
				if(opts[i].text.startsWith(value)) { 
					control.selectpicker('val', opts[i].text);
					break;
				}
			}
		}

	});

	if(typeof button !== 'undefined') {
		$(button).click();
	}

}

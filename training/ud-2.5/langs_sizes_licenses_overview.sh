sed 's/^\([^_]*\)[^ ]* \([^ ]*\)/\1 \2 &/' $(dirname $0)/langs_sizes_licenses | sort -nrk2 | sort -s -k1,1 | cut -d" " -f1,3- | sed 's/License:/ &/; s/ License:.*NC/NC&/; s/ /\t/; s/ /\t/; s/ /\t/;s/ /\t/' | column -nts "	"


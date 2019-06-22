
CREATE FUNCTION levenshtein_distance(text, text) RETURNS integer 
	AS 'similarity_join', 'levenshtein_distance' 
	LANGUAGE C STRICT;

CREATE FUNCTION jaccard_index(text, text) RETURNS double precision 
	AS 'similarity_join', 'jaccard_index' 
	LANGUAGE C STRICT;



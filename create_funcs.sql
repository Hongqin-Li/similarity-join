
DROP FUNCTION IF EXISTS levenshtein_distance(text, text);
DROP FUNCTION IF EXISTS levenshtein_distance_less_than(text, text, integer);
DROP FUNCTION IF EXISTS jaccard_index(text, text);

CREATE FUNCTION levenshtein_distance(text, text) RETURNS integer 
	AS 'similarity_join', 'levenshtein_distance' 
	LANGUAGE C STRICT;

CREATE FUNCTION levenshtein_distance_less_than(text, text, integer) RETURNS boolean
	AS 'similarity_join', 'levenshtein_distance_less_than' 
	LANGUAGE C STRICT;

CREATE FUNCTION jaccard_index(text, text) RETURNS double precision 
	AS 'similarity_join', 'jaccard_index' 
	LANGUAGE C STRICT;



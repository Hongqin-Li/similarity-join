
SELECT COUNT(*) 
FROM restaurantphone rp, addressphone ap 
WHERE levenshtein_distance_less_than(rp.phone, ap.phone, 4);

SELECT COUNT(*)
FROM restaurantaddress ra, restaurantphone rp
WHERE levenshtein_distance_less_than(ra.name, rp.name, 3);

SELECT COUNT(*) 
FROM restaurantaddress ra, addressphone ap 
WHERE levenshtein_distance_less_than(ra.address, ap.address, 4);

SELECT COUNT(*) 
FROM restaurantphone rp, addressphone ap 
WHERE levenshtein_distance(rp.phone, ap.phone) < 4;

SELECT COUNT(*)
FROM restaurantaddress ra, restaurantphone rp
WHERE levenshtein_distance(ra.name, rp.name) < 3;

SELECT COUNT(*) 
FROM restaurantaddress ra, addressphone ap 
WHERE levenshtein_distance(ra.address, ap.address) < 4;

SELECT COUNT(*)
FROM restaurantphone rp, addressphone ap
WHERE jaccard_index(rp.phone, ap.phone) > 0.6;

SELECT COUNT(*)
FROM restaurantaddress ra, restaurantphone rp
WHERE jaccard_index(ra.name, rp.name) > 0.65;

SELECT COUNT(*)
FROM restaurantaddress ra, addressphone ap
WHERE jaccard_index(ra.address, ap.address) > 0.8;


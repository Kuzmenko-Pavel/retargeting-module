SELECT id
FROM Offer
WHERE accountId=lower('%q') AND retargetingId=%s
LIMIT 1
;

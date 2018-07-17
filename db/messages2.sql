
create table lead_source (
    _id varchar(100),
    first_name varchar(255),
    last_name varchar(255),
    email varchar(255),
    email varchar(255) unique,
    phone_number varchar(20),
    source varchar(20),
    apply_date timestamp,
    message_id varchar(100),
    submitted_to_vipkid integer
)

-- INSERT INTO lead_source (_id,first_name,last_name,email,phone_number,source,apply_date,message_id,submitted_to_vipkid)
-- VALUES('1','1','1','1','1','1','2018-01-01','1','0');


